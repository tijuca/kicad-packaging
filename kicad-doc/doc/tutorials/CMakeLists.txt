install(DIRECTORY de
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-de
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)

install(DIRECTORY en
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-en
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)

install(DIRECTORY es
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-es
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)

install(DIRECTORY fr
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-fr
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)

install(DIRECTORY hu
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-hu
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)

install(DIRECTORY ru
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-ru
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)

install(DIRECTORY zh_CN
        DESTINATION ${KICAD_DOCS}
        COMPONENT doc-zh_CN
        PATTERN ".svn" EXCLUDE
        PATTERN "*.odt" EXCLUDE
        PATTERN "*.sxw" EXCLUDE)
