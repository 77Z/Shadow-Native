project('WIS 77Z UserCode', 'cpp',

  version: '0.1.0',
  license: 'Closed IDK',
  default_options: ['default_library=both']
)

#add_project_arguments('-s', language: 'cpp')

inc = include_directories(['include', '../include/API'])

library('usercode', ['code.cpp'], include_directories : inc)
