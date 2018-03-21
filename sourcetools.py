# -*- coding: utf-8 -*-
"""
(c) Copyright 2018, Sensirion AG, Switzerland
"""
import re


class SourceSquash(object):
    """ Squash merge files """

    def __init__(self, print_file_separator=True, version_tag=''):
        self.copyright = []
        self.system_includes = {}
        self.project_includes = {}
        self.lines = []
        self.print_file_separator = print_file_separator
        # only match system includes with angled brackets: <>
        self.system_include_pattern = re.compile(r'\s*#include\s*<(.*)>')
        self.project_include_pattern = re.compile(r'\s*#include\s*"(.*)"')
        # Match function definitions: "const long int *foo(..\n..)"
        # first group is return type (up to function name)
        ident = r'[a-zA-Z_*]\w*'  # one identifier
        sp = r'\s+'  # nonempty space
        self.function_pattern = re.compile('((' + ident + sp + ')+)' + ident +
                                           r'\s*\(.*$')
        self.version_tag = version_tag
        self.inject_version_tag = bool(version_tag)

    def add_project_include(self, include):
        self.project_includes[include] = True

    def readfile(self, filename, make_static=True, use_copyright=False,
                 substitutions=None):
        """ Read a source file

        filename -- read file with given path

        Keyword arguments:
        make_static -- make c functions static (default True)
        use_copyright -- use the copyright header from this file (default False)
        substitutions -- List of tuples of form (search_re, replacement):
                         compiled search regex and replacement string
                         to be applied as search_re.sub(replacement, line)
        """
        if use_copyright:
            self.copyright = []
        with open(filename, 'r') as fin:
            line = fin.readline()
            if not line:
                return

            # Parse Copyright Header
            if line.startswith('/*'):
                while line:
                    if use_copyright:
                        if substitutions:
                            for (search_re, repl) in substitutions:
                                line = search_re.sub(repl, line)
                        self.copyright.append(line.rstrip())
                    if '*/' in line:
                        break
                    line = fin.readline()

            if not line:
                return

            # Parse rest of file (source)
            self.lines.append('')
            self.lines.append('')
            if (self.print_file_separator):
                self.lines.append('/* =========== ' + filename + ' =========== */')

            for line in fin:
                line = line.rstrip()

                # Apply substitutions
                if substitutions:
                    for (search_re, repl) in substitutions:
                        line = search_re.sub(repl, line)

                # Handle includes
                sys_inc_match = self.system_include_pattern.match(line)
                if sys_inc_match:
                    include = sys_inc_match.group(1)
                    if include != 'assert.h':  # assert.h is #ifdef'ed
                        self.system_includes[include] = True
                        continue
                elif self.project_include_pattern.match(line):
                    continue

                if make_static:
                    line = self._make_static(line)
                self.lines.append(line)

    def write_stream(self, fout):
        self._write_copyright(fout)
        self._write_includes(fout)
        self._write_sources(fout)

    def write(self, out_path):
        with open(out_path, 'w') as fout:
            self.write_stream(fout)

    def _make_static(self, line):
        match = self.function_pattern.match(line)
        if match and 'static ' not in match.group(1):
            line = 'static ' + match.group(0)
        return line

    def _write_copyright(self, fout):
        if self.inject_version_tag:
            fout.write('/* SHT Driver Version: ' + self.version_tag + '\n *\n')
        for line in self.copyright:
            if line.startswith('/*') and self.inject_version_tag:
                if len(line) > 2:
                    line = ' ' + line[1:]
                else:
                    continue
            fout.write(line)
            fout.write('\n')
        fout.write('\n')

    def _write_includes(self, fout):
        for include in self.system_includes.keys():
            fout.write('#include <' + include + '>\n')
        if len(self.project_includes) > 0:
            fout.write('\n')
        for include in self.project_includes.keys():
            fout.write('#include "' + include + '"\n')

    def _write_sources(self, fout):
        for line in self.lines:
            fout.write(line)
            fout.write('\n')
