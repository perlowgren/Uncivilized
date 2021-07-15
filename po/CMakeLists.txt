
# CMakeLists.txt
#


message("Building language library.")

set(LANGUAGES
	en
	sv
)


find_program(GETTEXT_MSGINIT_EXECUTABLE msginit)
if(NOT GETTEXT_MSGINIT_EXECUTABLE)
	message(FATAL_ERROR "msginit not found")
endif()

find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
if(NOT GETTEXT_XGETTEXT_EXECUTABLE )
	message(FATAL_ERROR "xgettext not found")
endif()

find_program(GETTEXT_MSGCAT_EXECUTABLE msgcat)
if(NOT GETTEXT_MSGCAT_EXECUTABLE )
	message(FATAL_ERROR "msgcat not found")
endif()

find_program(GETTEXT_MSGATTRIB_EXECUTABLE msgattrib)
if(NOT GETTEXT_MSGATTRIB_EXECUTABLE)
	message(FATAL_ERROR "msgattrib not found")
endif()


message("Generate pot file. GETTEXT_XGETTEXT_EXECUTABLE: ${GETTEXT_XGETTEXT_EXECUTABLE}")
add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/po/uncivilized.pot
	COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE}
		--force-po
		--add-comments=TRANSLATORS
		--files-from=${PROJECT_SOURCE_DIR}/po/POTFILES.in
		--copyright-holder=\"Per Löwgren\"
		--msgid-bugs-address=\"per.lowgren@gmail.com\"
		--from-code=UTF-8
		--sort-by-file
		--keyword=_
		--output=${PROJECT_SOURCE_DIR}/po/uncivilized.pot 
	DEPENDS ${PROJECT_SOURCE_DIR}/po/POTFILES.in
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMENT "pot-update [uncivilized]: Generated source pot file."
)

foreach(LANG ${LANGUAGES})
	message("Generate ${LANG}.po file.")
	if(NOT EXISTS ${PROJECT_SOURCE_DIR}/po/${LANG}.po)
		add_custom_command(
			OUTPUT ${PROJECT_SOURCE_DIR}/po/${LANG}.po
			COMMAND ${GETTEXT_MSGINIT_EXECUTABLE}
				--no-translator 
				--input=uncivilized.pot 
				--output-file=${LANG}.po
				--locale=${LANG}
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
			DEPENDS ${PROJECT_SOURCE_DIR}/po/uncivilized.pot
			COMMENT "pot-update [${LANG}]: Initialized po file."
		)
	endif()

	add_custom_command(
		OUTPUT ${PROJECT_SOURCE_DIR}/po/${LANG}.po.dummy
		COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} 
			--backup=none
			-U ${LANG}.po 
			uncivilized.pot
		DEPENDS
			${PROJECT_SOURCE_DIR}/po/uncivilized.pot 
			${PROJECT_SOURCE_DIR}/po/${LANG}.po
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
		COMMENT "pot-update [${LANG}]: Updated po file."
	)

	set(pot-update-SRC ${pot-update-SRC} ${PROJECT_SOURCE_DIR}/po/${LANG}.po.dummy)
endforeach()

set(pot-update-SRC ${pot-update-SRC} ${PROJECT_SOURCE_DIR}/po/${LANG}.pot)
add_custom_target(po-update-LANG
	COMMENT "po-update: Done."
	DEPENDS ${po-update-SRC}
)

foreach(LANG ${LANGUAGES})
	message("Generate ${LANG}.mo file.")
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/po/locale/${LANG}
		COMMAND ${CMAKE_COMMAND} -E make_directory 
			${CMAKE_SOURCE_DIR}/po/locale/${LANG}
		COMMENT "mo-update [${LANG}]: Creating locale directory."
	)
	add_custom_command(
		OUTPUT ${PROJECT_SOURCE_DIR}/po/locale/${LANG}/uncivilized.mo
		COMMAND ${GETTEXT_MSGFMT_EXECUTABLE}
			-v -o ${CMAKE_SOURCE_DIR}/po/locale/${LANG}/uncivilized.mo ${LANG}.po
		DEPENDS 
			${CMAKE_SOURCE_DIR}/po/locale/${LANG}
			${PROJECT_SOURCE_DIR}/po/${LANG}.po
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
		COMMENT "mo-update [${LANG}]: Creating mo file."
	)

	install(FILES
		${PROJECT_SOURCE_DIR}/po/locale/${LANG}/uncivilized.mo
		DESTINATION ${LOCALEDIR}/${LANG}/LC_MESSAGES
	)

	set(mo-update-SRC ${mo-update-SRC} 
		${PROJECT_SOURCE_DIR}/po/locale/${LANG}/uncivilized.mo
	)
endforeach()

add_custom_target(mo-update ALL
	COMMENT "mo-update: Done."
	DEPENDS ${mo-update-SRC}
)

set_directory_properties(properties 
	ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/po/${LOCALEDIR}
)


