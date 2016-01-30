set(TutorialLangs de en es fr hu it ja pl ru zh_CN)

foreach(TutorialLang ${TutorialLangs})
	install(DIRECTORY ${TutorialLang}
		DESTINATION ${KICAD_HELP}
		COMPONENT doc-${TutorialLang}
		PATTERN ".svn" EXCLUDE
		PATTERN "*.odt" EXCLUDE
		PATTERN "*.sxw" EXCLUDE)
endforeach(TutorialLang)

