#include "modular_art_manager.h"
#include "curl.h" // EDOPro's curl wrapper
#include "file_stream.h"
#include "fmt.h"
#include "game_config.h"
#include "logging.h"
#include "utils.h"
#include <array>
#include <fstream>
#include <sstream>

namespace ygo {

// Helper struct for curl download
struct CurlPayload {
  FILE *stream;
  size_t header_written;
  std::array<uint8_t, 8> header;
};

// Image header constants
static constexpr std::array<uint8_t, 8> pngheader{0x89, 0x50, 0x4e, 0x47,
                                                  0x0d, 0x0a, 0x1a, 0x0a};
static constexpr std::array<uint8_t, 3> jpgheader{0xff, 0xd8, 0xff};

enum headerType : uint8_t {
  UNK_FILE,
  PNG_FILE,
  JPG_FILE,
};

static headerType ImageHeaderType(const std::array<uint8_t, 8> &header) {
  if (pngheader == header)
    return PNG_FILE;
  if (!memcmp(jpgheader.data(), header.data(), jpgheader.size()))
    return JPG_FILE;
  return UNK_FILE;
}

static size_t WriteCallback(char *ptr, size_t size, size_t nmemb,
                            void *userdata) {
  static constexpr auto header_size = sizeof(CurlPayload::header);
  auto data = static_cast<CurlPayload *>(userdata);
  const size_t nbytes = size * nmemb;
  if (data->header_written < header_size) {
    auto increase = std::min(nbytes, header_size - data->header_written);
    memcpy(&data->header[data->header_written], ptr, increase);
    data->header_written += increase;
    if (data->header_written == header_size &&
        ImageHeaderType(data->header) == UNK_FILE)
      return 0xffffffff;
  }
  FILE *out = data->stream;
  fwrite(ptr, 1, nbytes, out);
  return nbytes;
}

static epro::path_stringview
GetExtension(const std::array<uint8_t, 8> &header) {
  switch (ImageHeaderType(header)) {
  case PNG_FILE:
    return EPRO_TEXT(".png");
  case JPG_FILE:
    return EPRO_TEXT(".jpg");
  default:
    return EPRO_TEXT("");
  }
}

ModularArtManager::ModularArtManager(irr::IrrlichtDevice *device)
    : device(device), driver(device->getVideoDriver()),
      cacheDir(EPRO_TEXT("pics_modular")) {
  // Create cache directory if it doesn't exist
  Utils::MakeDirectory(cacheDir);
}

ModularArtManager::~ModularArtManager() {
  // Textures are managed by Irrlicht, just clear our cache map
  artCache.clear();
}

irr::video::ITexture *ModularArtManager::GetCardArt(uint32_t code,
                                                    bool preferHighRes) {
  ErrorLog("ModularArtManager::GetCardArt called for code {}", code);

  // Check cache first
  auto it = artCache.find(code);
  if (it != artCache.end() && it->second) {
    ErrorLog("ModularArtManager: Found in memory cache");
    return it->second;
  }

  // Try to load from disk cache
  irr::video::ITexture *cached = LoadFromCache(code);
  if (cached) {
    ErrorLog("ModularArtManager: Found in disk cache");
    artCache[code] = cached;
    return cached;
  }

  // Not cached, try synchronous download
  ErrorLog("ModularArtManager: Not in cache, attempting download...");
  bool success = false;
  std::string url = preferHighRes ? GetHighResURL(code) : GetStandardURL(code);
  ErrorLog("ModularArtManager: Trying URL: {}", url);
  success = DownloadAndCache(code, url);

  // If high-res failed and we prefer high-res, try standard
  if (!success && preferHighRes) {
    url = GetStandardURL(code);
    ErrorLog("ModularArtManager: High-res failed, trying standard: {}", url);
    success = DownloadAndCache(code, url);
  }

  if (success) {
    ErrorLog("ModularArtManager: Download succeeded, loading from cache");
    cached = LoadFromCache(code);
    if (cached) {
      artCache[code] = cached;
      return cached;
    }
  } else {
    ErrorLog("ModularArtManager: Download failed for code {}", code);
  }

  return nullptr;
}

bool ModularArtManager::IsArtCached(uint32_t code) const {
  // Check memory cache
  if (artCache.find(code) != artCache.end()) {
    return true;
  }

  // Check disk cache (try both .jpg and .png)
  auto jpgPath = epro::format(EPRO_TEXT("{}/{}.jpg"), cacheDir, code);
  auto pngPath = epro::format(EPRO_TEXT("{}/{}.png"), cacheDir, code);
  return Utils::FileExists(jpgPath) || Utils::FileExists(pngPath);
}

void ModularArtManager::DownloadArtAsync(
    uint32_t code, bool preferHighRes,
    std::function<void(irr::video::ITexture *)> callback) {
  // For now, this is synchronous - can be made async later
  std::string url = preferHighRes ? GetHighResURL(code) : GetStandardURL(code);
  bool success = DownloadAndCache(code, url);

  if (!success && preferHighRes) {
    url = GetStandardURL(code);
    success = DownloadAndCache(code, url);
  }

  if (callback) {
    irr::video::ITexture *texture = success ? LoadFromCache(code) : nullptr;
    callback(texture);
  }
}

void ModularArtManager::ClearCache() { artCache.clear(); }

epro::path_string ModularArtManager::GetCachePath(uint32_t code) const {
  // First check for .jpg (more common from remote)
  auto jpgPath = epro::format(EPRO_TEXT("{}/{}.jpg"), cacheDir, code);
  if (Utils::FileExists(jpgPath))
    return jpgPath;

  // Then check for .png
  auto pngPath = epro::format(EPRO_TEXT("{}/{}.png"), cacheDir, code);
  if (Utils::FileExists(pngPath))
    return pngPath;

  // Return the expected jpg path for new downloads
  return jpgPath;
}

std::string ModularArtManager::GetHighResURL(uint32_t code) const {
  // Using ygoprodeck.com for cropped art - daominah servers are down
  return epro::format(
      "https://images.ygoprodeck.com/images/cards_cropped/{}.jpg", code);
}

std::string ModularArtManager::GetStandardURL(uint32_t code) const {
  // Using ygoprodeck.com for cropped art - daominah servers are down
  return epro::format(
      "https://images.ygoprodeck.com/images/cards_cropped/{}.jpg", code);
}

bool ModularArtManager::DownloadAndCache(uint32_t code,
                                         const std::string &url) {
  auto curl = curl_easy_init();
  if (!curl) {
    ErrorLog("ModularArtManager: Failed to init curl");
    return false;
  }

  CurlPayload payload;
  char curl_error_buffer[CURL_ERROR_SIZE];

  // Setup curl options
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &payload);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  // Add User-Agent header to bypass Cloudflare protection
  curl_easy_setopt(
      curl, CURLOPT_USERAGENT,
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");

  if (gGameConfig->ssl_certificate_path.size() &&
      Utils::FileExists(
          Utils::ToPathString(gGameConfig->ssl_certificate_path))) {
    curl_easy_setopt(curl, CURLOPT_CAINFO,
                     gGameConfig->ssl_certificate_path.data());
  }

  // Create temp file path
  auto tempPath =
      epro::format(EPRO_TEXT("{}/temp_{}.download"), cacheDir, code);

  // Open file for writing
  auto fp = fileopen(tempPath.c_str(), "wb");
  if (!fp) {
    curl_easy_cleanup(curl);
    ErrorLog("ModularArtManager: Failed to open temp file for {}", code);
    return false;
  }

  // Setup payload
  payload.stream = fp;
  payload.header_written = 0;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  // Perform download
  CURLcode res = curl_easy_perform(curl);
  fclose(fp);

  if (res == CURLE_OK) {
    // Successfully downloaded, rename to final path with correct extension
    auto ext = GetExtension(payload.header);
    auto finalPath =
        epro::format(EPRO_TEXT("{}/{}{}"), cacheDir, code, ext.data());

    if (!Utils::FileMove(tempPath, finalPath)) {
      Utils::FileDelete(tempPath);
      curl_easy_cleanup(curl);
      return false;
    }

    curl_easy_cleanup(curl);
    return true;
  } else {
    // Download failed - always log for debugging
    Utils::FileDelete(tempPath);
    curl_easy_cleanup(curl);
    ErrorLog("ModularArtManager: Failed downloading art for {} from {}", code,
             url);
    ErrorLog("Curl error: ({}) {} ({})", static_cast<int>(res),
             curl_easy_strerror(res), curl_error_buffer);
    return false;
  }
}

irr::video::ITexture *ModularArtManager::LoadFromCache(uint32_t code) {
  auto cachePath = GetCachePath(code);

  if (!Utils::FileExists(cachePath)) {
    return nullptr;
  }

  // Generate a unique texture name to avoid conflicts
  auto textureName = epro::format(EPRO_TEXT("modular_art_{}"), code);

  irr::video::ITexture *texture = driver->getTexture(cachePath.c_str());
  return texture;
}

} // namespace ygo
