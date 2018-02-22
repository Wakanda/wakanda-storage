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
		"defines": [
			"BOOST_DATE_TIME_NO_LIB"
		]
	}]
}