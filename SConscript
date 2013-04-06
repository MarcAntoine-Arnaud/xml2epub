Import( 'project', 'libs' )


### Define global flags for the whole project
# depending on the platform and compilation mode
xml2epubFlags = {
		'LIBPATH': [project.inOutputLib()],
		'CCFLAGS': project.CC['warning3'],
		'CPPDEFINES':[],
	}

if project.env['mode'] == 'production' :
	# In 'production' mode set a flag XML2EPUB_PRODUCTION
	xml2epubFlags['CPPDEFINES'].append( 'XML2EPUB_PRODUCTION' )
	# If your compiler as a visibility flag, hide all plugin internal things
	if 'visibilityhidden' in project.CC:
		xml2epubFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

# If your compiler as a flag to mark undefined flags as error in shared libraries
if 'sharedNoUndefined' in project.CC:
	xml2epubFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]

# Creates a dependency target without associated code or compiled object,
# but only associated with compilation flags
xml2epub = project.ObjectLibrary( 'xml2epubDefault', envFlags=xml2epubFlags )
# Set this object library as a default library for all targets
project.commonLibs.append( xml2epub )



### Load all SConscript files (in the correct order)
SConscript(
		project.scanFiles( [
				'libraries/common',
				'libraries/converter',
				'apps',
			], accept=['SConscript'] )
	)


