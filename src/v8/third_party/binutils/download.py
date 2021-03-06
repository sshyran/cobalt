#!/usr/bin/env python
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
# vim: set ts=2 sw=2 et sts=2 ai:

"""Minimal tool to download binutils from Google storage.

TODO(mithro): Replace with generic download_and_extract tool.
"""


from __future__ import print_function
import os
import platform
import re
import shutil
import subprocess
import sys
from detect_v8_host_arch import DetectHostArch


BINUTILS_DIR = os.path.abspath(os.path.dirname(__file__))
BINUTILS_FILE = 'binutils.tar.bz2'
BINUTILS_TOOLS = ['bin/ld.gold', 'bin/objcopy', 'bin/objdump']
BINUTILS_OUT = 'Release'


def ReadFile(filename):
  with open(filename, 'r') as f:
    return f.read().strip()


def WriteFile(filename, content):
  assert not os.path.exists(filename)
  with open(filename, 'w') as f:
    f.write(content)
    f.write('\n')


def FetchAndExtract(arch):
  archdir = os.path.join(BINUTILS_DIR, 'Linux_' + arch)
  tarball = os.path.join(archdir, BINUTILS_FILE)
  outdir = os.path.join(archdir, BINUTILS_OUT)

  sha1file = tarball + '.sha1'
  if not os.path.exists(sha1file):
    print("WARNING: No binutils found for your architecture (%s)!" % arch)
    return 0

  checksum = ReadFile(sha1file)

  stampfile = tarball + '.stamp'
  if os.path.exists(stampfile):
    if (os.path.exists(tarball) and
        os.path.exists(outdir) and
        checksum == ReadFile(stampfile)):
      return 0
    else:
      os.unlink(stampfile)

  print("Downloading", tarball)
  subprocess.check_call([
      'download_from_google_storage',
      '--no_resume',
      '--no_auth',
      '--bucket', 'chromium-binutils',
      '-s', sha1file])
  assert os.path.exists(tarball)

  if os.path.exists(outdir):
    shutil.rmtree(outdir)
  assert not os.path.exists(outdir)
  os.makedirs(outdir)
  assert os.path.exists(outdir)

  print("Extracting", tarball)
  subprocess.check_call(['tar', 'axf', tarball], cwd=outdir)

  for tool in BINUTILS_TOOLS:
    assert os.path.exists(os.path.join(outdir, tool))

  WriteFile(stampfile, checksum)
  return 0


def main(args):
  if not sys.platform.startswith('linux'):
    return 0

  arch = DetectHostArch()
  if arch == 'x64':
    return FetchAndExtract(arch)
  if arch == 'x86':
    ret = FetchAndExtract('ia32')
    if ret != 0:
      return ret
    # Fetch the x64 toolchain as well for official bots with 64-bit kernels.
    return FetchAndExtract('x64')
  return 0


if __name__ == '__main__':
  sys.exit(main(sys.argv))
