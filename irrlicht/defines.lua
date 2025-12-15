-- Irrlicht compilation defines
defines {
	"_IRR_STATIC_LIB_",
	"_CRT_SECURE_NO_DEPRECATE",
	"IRRLICHT_EXPORTS=1"
}

filter "system:windows"
	defines "_IRR_WCHAR_FILESYSTEM"
