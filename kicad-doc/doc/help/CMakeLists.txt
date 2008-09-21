# install(DIRECTORY de
#         DESTINATION ${KICAD_HELP}
#         COMPONENT help-de
#         PATTERN ".svn" EXCLUDE
#         PATTERN "docs_src" EXCLUDE)

install(DIRECTORY en
        DESTINATION ${KICAD_HELP}
        COMPONENT help-en
        PATTERN ".svn" EXCLUDE
        PATTERN "docs_src" EXCLUDE)

# install(DIRECTORY es
#         DESTINATION ${KICAD_HELP}
#         COMPONENT help-es
#         PATTERN ".svn" EXCLUDE
#         PATTERN "docs_src" EXCLUDE)

install(DIRECTORY fr
        DESTINATION ${KICAD_HELP}
        COMPONENT help-fr
        PATTERN ".svn" EXCLUDE
        PATTERN "docs_src" EXCLUDE)

# install(DIRECTORY it
#         DESTINATION ${KICAD_HELP}
#         COMPONENT help-it
#         PATTERN ".svn" EXCLUDE
#         PATTERN "docs_src" EXCLUDE)

# install(DIRECTORY pt
#         DESTINATION ${KICAD_HELP}
#         COMPONENT help-pt
#         PATTERN ".svn" EXCLUDE
#         PATTERN "docs_src" EXCLUDE)

# install(DIRECTORY ru
#         DESTINATION ${KICAD_HELP}
#         COMPONENT help-ru
#         PATTERN ".svn" EXCLUDE
#         PATTERN "docs_src" EXCLUDE)

install(DIRECTORY file_formats
        DESTINATION ${KICAD_HELP}
        COMPONENT file_formats
        PATTERN ".svn" EXCLUDE
        PATTERN "docs_src" EXCLUDE)

install(DIRECTORY footprints_doc
        DESTINATION ${KICAD_HELP}
        COMPONENT footprints_doc
        PATTERN ".svn" EXCLUDE)
