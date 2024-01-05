#!/usr/bin/env python3

# Copyright 2018 The Meson development team
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

import os
import tempfile
import unittest
import subprocess
import zipapp
from pathlib import Path

from mesonbuild.mesonlib import windows_proof_rmtree, python_command, is_windows
from mesonbuild.coredata import version as meson_version


def get_pypath():
    import sysconfig
    pypath = sysconfig.get_path('purelib', vars={'base': ''})
    # Ensure that / is the path separator and not \, then strip /
    return Path(pypath).as_posix().strip('/')

def get_pybindir():
    import sysconfig
    # 'Scripts' on Windows and 'bin' on other platforms including MSYS
    return sysconfig.get_path('scripts', vars={'base': ''}).strip('\\/')

class CommandTests(unittest.TestCase):
    '''
    Test that running meson in various ways works as expected by checking the
    value of mesonlib.meson_command that was set during configuration.
    '''

    def setUp(self):
        super().setUp()
        self.orig_env = os.environ.copy()
        self.orig_dir = os.getcwd()
        os.environ['MESON_COMMAND_TESTS'] = '1'
        self.tmpdir = Path(tempfile.mkdtemp()).resolve()
        self.src_root = Path(__file__).resolve().parent
        self.testdir = str(self.src_root / 'test cases/common/1 trivial')
        self.meson_args = ['--backend=ninja']

    def tearDown(self):
        try:
            windows_proof_rmtree(str(self.tmpdir))
        except FileNotFoundError:
            pass
        os.environ.clear()
        os.environ.update(self.orig_env)
        os.chdir(str(self.orig_dir))
        super().tearDown()

    def _run(self, command, workdir=None):
        '''
        Run a command while printing the stdout, and also return a copy of it
        '''
        # If this call hangs CI will just abort. It is very hard to distinguish
        # between CI issue and test bug in that case. Set timeout and fail loud
        # instead.
        p = subprocess.run(command, stdout=subprocess.PIPE,
                           env=os.environ.copy(), universal_newlines=True,
                           cwd=workdir, timeout=60 * 5)
        print(p.stdout)
        if p.returncode != 0:
            raise subprocess.CalledProcessError(p.returncode, command)
        return p.stdout

    def assertMesonCommandIs(self, line, cmd):
        self.assertTrue(line.startswith('meson_command '), msg=line)
        self.assertEqual(line, f'meson_command is {cmd!r}')

    def test_meson_uninstalled(self):
        # This is what the meson command must be for all these cases
        resolved_meson_command = python_command + [str(self.src_root / 'meson.py')]
        # Absolute path to meson.py
        os.chdir('/')
        builddir = str(self.tmpdir / 'build1')
        meson_py = str(self.src_root / 'meson.py')
        meson_setup = [meson_py, 'setup']
        meson_command = python_command + meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)
        # ./meson.py
        os.chdir(str(self.src_root))
        builddir = str(self.tmpdir / 'build2')
        meson_py = './meson.py'
        meson_setup = [meson_py, 'setup']
        meson_command = python_command + meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)
        # Symlink to meson.py
        if is_windows():
            # Symlinks require admin perms
            return
        os.chdir(str(self.src_root))
        builddir = str(self.tmpdir / 'build3')
        # Create a symlink to meson.py in bindir, and add it to PATH
        bindir = (self.tmpdir / 'bin')
        bindir.mkdir()
        (bindir / 'meson').symlink_to(self.src_root / 'meson.py')
        os.environ['PATH'] = str(bindir) + os.pathsep + os.environ['PATH']
        # See if it works!
        meson_py = 'meson'
        meson_setup = [meson_py, 'setup']
        meson_command = meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)

    def test_meson_installed(self):
        # Install meson
        prefix = self.tmpdir / 'prefix'
        pylibdir = prefix / get_pypath()
        bindir = prefix / get_pybindir()
        pylibdir.mkdir(parents=True)
        # XXX: join with empty name so it always ends with os.sep otherwise
        # distutils complains that prefix isn't contained in PYTHONPATH
        os.environ['PYTHONPATH'] = os.path.join(str(pylibdir), '')
        os.environ['PATH'] = str(bindir) + os.pathsep + os.environ['PATH']
        self._run(python_command + ['setup.py', 'install', '--prefix', str(prefix)])
        # Fix importlib-metadata by appending all dirs in pylibdir
        PYTHONPATHS = [pylibdir] + [x for x in pylibdir.iterdir()]
        PYTHONPATHS = [os.path.join(str(x), '') for x in PYTHONPATHS]
        os.environ['PYTHONPATH'] = os.pathsep.join(PYTHONPATHS)
        # Check that all the files were installed correctly
        self.assertTrue(bindir.is_dir())
        self.assertTrue(pylibdir.is_dir())
        # Run `meson`
        os.chdir('/')
        resolved_meson_command = [str(bindir / 'meson')]
        builddir = str(self.tmpdir / 'build1')
        meson_setup = ['meson', 'setup']
        meson_command = meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)
        # Run `/path/to/meson`
        builddir = str(self.tmpdir / 'build2')
        meson_setup = [str(bindir / 'meson'), 'setup']
        meson_command = meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)
        # Run `python3 -m mesonbuild.mesonmain`
        resolved_meson_command = python_command + ['-m', 'mesonbuild.mesonmain']
        builddir = str(self.tmpdir / 'build3')
        meson_setup = ['-m', 'mesonbuild.mesonmain', 'setup']
        meson_command = python_command + meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)
        if is_windows():
            # Next part requires a shell
            return
        # `meson` is a wrapper to `meson.real`
        resolved_meson_command = [str(bindir / 'meson.real')]
        builddir = str(self.tmpdir / 'build4')
        (bindir / 'meson').rename(bindir / 'meson.real')
        wrapper = (bindir / 'meson')
        wrapper.write_text('#!/bin/sh\n\nmeson.real "$@"', encoding='utf-8')
        wrapper.chmod(0o755)
        meson_setup = [str(wrapper), 'setup']
        meson_command = meson_setup + self.meson_args
        stdo = self._run(meson_command + [self.testdir, builddir])
        self.assertMesonCommandIs(stdo.split('\n')[0], resolved_meson_command)

    def test_meson_exe_windows(self):
        raise unittest.SkipTest('NOT IMPLEMENTED')

    def test_meson_zipapp(self):
        if is_windows():
            raise unittest.SkipTest('NOT IMPLEMENTED')
        source = Path(__file__).resolve().parent
        target = self.tmpdir / 'meson.pyz'
        script = source / 'packaging' / 'create_zipapp.py'
        self._run([script.as_posix(), source, '--outfile', target, '--interpreter', python_command[0]])
        self._run([target.as_posix(), '--help'])


if __name__ == '__main__':
    print('Meson build system', meson_version, 'Command Tests')
    raise SystemExit(unittest.main(buffer=True))
