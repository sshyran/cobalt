# Copyright 2020 The Cobalt Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'targets': [
    {
      'target_name': 'app_key_files',
      'type': 'static_library',
      'sources': [
        'app_key_files.h',
        'app_key_files.cc',
       ],
      'dependencies' : [
        '<(DEPTH)/starboard/starboard.gyp:starboard',
      ],
    },
    {
      'target_name': 'app_key_files_test',
      'type': '<(gtest_target_type)',
      'sources': [
        'app_key_files_test.cc',
        '<(DEPTH)/starboard/common/test_main.cc',
      ],
      'dependencies': [
         ':app_key_files',
         '<(DEPTH)/testing/gmock.gyp:gmock',
         '<(DEPTH)/testing/gtest.gyp:gtest',
      ],
    },
    {
      'target_name': 'app_key_files_test_deploy',
      'type': 'none',
      'dependencies': [
        'app_key_files_test',
      ],
      'variables': {
        'executable_name': 'app_key_files_test',
      },
      'includes': [ '<(DEPTH)/starboard/build/deploy.gypi' ],
    },
  ],
}
