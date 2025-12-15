#ifndef MODULAR_ART_MANAGER_H
#define MODULAR_ART_MANAGER_H

#include "config.h"
#include "text_types.h"
#include <functional>
#include <irrlicht.h>
#include <map>
#include <string>

namespace ygo {

// Art download manager for modular card renderer
class ModularArtManager {
public:
  ModularArtManager(irr::IrrlichtDevice *device);
  ~ModularArtManager();

  // Download strategy from handoff doc
  // High-res: https://mdygo2048.daominah.uk/{code}.jpg
  // Standard: https://mdygo.daominah.uk/{code}.jpg

  // Get card art texture (async download if not cached)
  irr::video::ITexture *GetCardArt(uint32_t code, bool preferHighRes = true);

  // Check if art is cached locally
  bool IsArtCached(uint32_t code) const;

  // Download art asynchronously (non-blocking)
  void DownloadArtAsync(
      uint32_t code, bool preferHighRes,
      std::function<void(irr::video::ITexture *)> callback = nullptr);

  // Clear cache
  void ClearCache();

  // Get cache directory
  const epro::path_string &GetCacheDir() const { return cacheDir; }

private:
  irr::IrrlichtDevice *device;
  irr::video::IVideoDriver *driver;

  // Cache location: pics_modular/{code}.png
  epro::path_string cacheDir;

  // Cached textures
  std::map<uint32_t, irr::video::ITexture *> artCache;

  // Helper functions
  epro::path_string GetCachePath(uint32_t code) const;
  std::string GetHighResURL(uint32_t code) const;
  std::string GetStandardURL(uint32_t code) const;

  // Download and save to cache
  bool DownloadAndCache(uint32_t code, const std::string &url);

  // Load from cache
  irr::video::ITexture *LoadFromCache(uint32_t code);
};

} // namespace ygo

#endif // MODULAR_ART_MANAGER_H
