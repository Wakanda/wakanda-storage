{
	"targets": [{
		"target_name": "wakanda_storage",
		"sources": [
			"src/addon_entry_point.cpp",
			"src/shared_storage.cpp",
			"src/napi_helpers.cpp"
		],
		
		"include_dirs": [
			"src",
			"deps"
		],
		"cflags_cc!": ["-fno-rtti", "-fno-exceptions"],
		"cflags_cc+": ["-frtti", "-fexceptions"],
		"conditions": [[
			"OS==\"mac\"", {
				"xcode_settings": {
					"OTHER_CPLUSPLUSFLAGS": ["-std=c++11", "-stdlib=libc++"],
					"OTHER_LDFLAGS": ["-stdlib=libc++"],
					"GCC_ENABLE_CPP_EXCEPTIONS": "YES",
					"GCC_ENABLE_CPP_RTTI": "YES"
				}
			}
		]],
		"defines": [
			"BOOST_DATE_TIME_NO_LIB"
		]
	},
	{
		"target_name": "action_after_build",
		"type": "none",
		"dependencies": [ "<(module_name)" ],
		"copies": [{
		   "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
		   "destination": "<(module_path)"
        }]
    }
	
	]
}