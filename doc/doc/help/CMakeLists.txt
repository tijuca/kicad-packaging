set(HelpLangs de en es fr it ja pl pt ru)

foreach(HelpLang ${HelpLangs})
	install(DIRECTORY ${HelpLang}
		DESTINATION ${KICAD_HELP}
		COMPONENT help-${HelpLang}
		PATTERN ".svn" EXCLUDE
		PATTERN "docs_src" EXCLUDE)
endforeach(HelpLang)

install(DIRECTORY file_formats
	DESTINATION ${KICAD_HELP}
	COMPONENT file_formats
	PATTERN ".svn" EXCLUDE
	PATTERN "docs_src" EXCLUDE)

