{
	"targets": [{
		"target_name": "wakanda_storage",
		"sources": [
			"src/addon_entry_point.cpp",
			"src/shared_storage.h",
			"src/shared_storage.cpp",
			"src/shared_item.h",
			"src/js_shared_storage.h",
			"src/js_shared_storage.cpp",
			"src/napi_helpers.cpp"
		],
		
		"include_dirs": [
			"src",
			"deps/boost"
		],
		"cflags!": ["-fno-rtti", "-fno-exceptions"],
		"cflags_cc!": ["-fno-rtti", "-fno-exceptions"],
		"conditions": [[
			"OS==\"mac\"", {
				"xcode_settings": {
					"OTHER_CPLUSPLUSFLAGS": ["-std=c++14", "-stdlib=libc++"],
					"OTHER_LDFLAGS": ["-stdlib=libc++"],
					"GCC_ENABLE_CPP_EXCEPTIONS": "YES",
					"GCC_ENABLE_CPP_RTTI": "YES"
				}
			}
		],[
			"OS==\"win\"", {
				"msvs_settings": {
		    		"VCCLCompilerTool": {
		    			"ExceptionHandling": 1
		    		}
		  		},
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