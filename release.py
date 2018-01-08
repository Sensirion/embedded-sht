#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
(c) Copyright 2017, Sensirion AG, Switzerland
"""

import argparse
import os
import sys
import re
try:
    import git
    use_git = True
except:
    use_git = False

from sourcetools import SourceSquash

RELEASE_TAG_FILE = 'git_release_tag.txt'
EMPTY_TAG_STRING = ''
SLA_HEADER_PATH = 'licences/sla_copyright.txt'

class SHTDriverRelease(object):

    def __init__(self):
        self.copy_files = []
        self.example_source_files = ['example_usage.c']
        self.driver_header = 'sht.h'
        self.sensirion_configuration_header = 'embedded-common/sensirion_arch_config.h'
        self.driver_configuration_header = 'sensirion_arch_config.h'
        # Files to be merged with the driver source
        self.source_merge_files = ['sht_common.c']  # Added before original source

        # Closed-source files to be merged with the driver.
        # If any of the two following lists contains at least one item --> use SLA header
        # Otherwise use original (open-source) header
        self.engine_source_merge_files = []  # Added after original source
        self.engine_header_merge_files = []  # Added after original source

        # Force closed source header
        self.force_proprietary_license = False

        self.copyright_header_path = ''

        self.merge_file_per_sensor = {
            'shtc1': 'shtc1.c',
            'shtw2': 'shtc1.c',
            'sht3x': 'sht3x.c',
        }

        self.header_substitutions_per_sensor = {
            'sht3x': [(re.compile('SHT_H'), 'SHT3X_H'), (re.compile('\[###SENSOR###]'), 'SHT3x')],
            'shtc1': [(re.compile('SHT_H'), 'SHTC1_H'), (re.compile('\[###SENSOR###]'), 'SHTC1')],
            'shtw2': [(re.compile('SHT_H'), 'SHTW2_H'), (re.compile('\[###SENSOR###]'), 'SHTW2')]
        }

        self.source_substitutions_per_sensor = {
            'sht3x': [(re.compile('\[###SENSOR###]'), 'SHT3x')],
            'shtc1': [(re.compile('\[###SENSOR###]'), 'SHTC1')],
            'shtw2': [(re.compile('SHTC1'), 'SHTW2'), (re.compile('\[###SENSOR###]'), 'SHTW2')]
        }

        self.other_substitutions_per_sensor = {
            'sht3x': [(re.compile('\[###SENSOR###]'), 'SHT3x')],
            'shtc1': [(re.compile('\[###SENSOR###]'), 'SHTC1')],
            'shtw2': [(re.compile('\[###SENSOR###]'), 'SHTW2')]
        }

        self.files_per_i2c_type = {
            'hw_i2c': {
                'source_merge': [],
                'configuration_header_merge': ['embedded-common/sensirion_i2c.h'],
                'configuration_source': 'embedded-common/hw_i2c/sensirion_hw_i2c_implementation.c',
            },
            'sw_i2c': {
                'source_merge': ['embedded-common/sw_i2c/sensirion_sw_i2c.c'],
                'configuration_header_merge': ['embedded-common/sw_i2c/sensirion_sw_i2c_gpio.h'],
                'configuration_source': 'embedded-common/sw_i2c/sensirion_sw_i2c_implementation.c',
            },
        }

    def release_configuration(self, sensor, i2c_type, output_directory):
        """
        Release one possible configuration of the generic driver and write it to a directory.
        """

        # set path to SLA header. If this file is unavailable
        # --> raise exception and do not generate any output file!
        if self.force_proprietary_license or self.engine_header_merge_files or self.engine_source_merge_files:
            self.copyright_header_path = SLA_HEADER_PATH
            if not os.path.isfile(self.copyright_header_path):
                raise IOError("The SLA header couldn't be found!")

        # Get the version tag to write to every source and header file
        version_tag = self.get_version_tag()

        if not os.path.exists(output_directory):
            os.makedirs(output_directory)

        # Generate shtxx.c source file
        driver_source = SourceSquash(print_file_separator=False, version_tag=version_tag)
        for file_name in self.files_per_i2c_type[i2c_type]['source_merge'] + self.source_merge_files:
            driver_source.readfile(file_name)
        substitutions = None
        if self.source_substitutions_per_sensor.has_key(sensor):
            substitutions = self.source_substitutions_per_sensor[sensor]
        use_copyright = True
        if self.copyright_header_path:
            driver_source.readfile(self.copyright_header_path, use_copyright=True, substitutions=substitutions)
            use_copyright = False
        driver_source.readfile(self.merge_file_per_sensor[sensor],
                               make_static=False, use_copyright=use_copyright, substitutions=substitutions)
        for file_name in self.engine_source_merge_files:
            driver_source.readfile(file_name, make_static=False)
        driver_source_destination = '{0}.c'.format(sensor)

        # Generate shtxx.h header file
        driver_header = SourceSquash(print_file_separator=False, version_tag=version_tag)
        substitutions = None
        if self.header_substitutions_per_sensor.has_key(sensor):
            substitutions = self.header_substitutions_per_sensor[sensor]
        use_copyright = True
        if self.copyright_header_path:
            driver_header.readfile(self.copyright_header_path, use_copyright=True, substitutions=substitutions)
            use_copyright = False
        driver_header.readfile(self.driver_header, make_static=False,
                               use_copyright=use_copyright, substitutions=substitutions)
        for file_name in self.engine_header_merge_files:
            driver_header.readfile(file_name, make_static=False)
        driver_header_destination = '{0}.h'.format(sensor)
        driver_source.add_project_include(driver_header_destination)

        # Generate sensirion_configuration.h header file
        configuration_header = SourceSquash(print_file_separator=False, version_tag=version_tag)
        substitutions = None
        if self.other_substitutions_per_sensor.has_key(sensor):
            substitutions = self.other_substitutions_per_sensor[sensor]
        use_copyright = True
        if self.copyright_header_path:
            configuration_header.readfile(self.copyright_header_path, use_copyright=True, substitutions=substitutions)
            use_copyright = False
        configuration_header.readfile(self.sensirion_configuration_header,
                                      make_static=False, use_copyright=use_copyright)
        for file_name in self.files_per_i2c_type[i2c_type]['configuration_header_merge']:
            configuration_header.readfile(file_name, make_static=False)
        driver_header.add_project_include(self.driver_configuration_header)

        # Generate sensirion_configuration.c source file
        configuration_source = SourceSquash(print_file_separator=False, version_tag=version_tag)
        substitutions = None
        if self.other_substitutions_per_sensor.has_key(sensor):
            substitutions = self.other_substitutions_per_sensor[sensor]
        use_copyright = True
        if self.copyright_header_path:
            configuration_source.readfile(self.copyright_header_path, use_copyright=True, substitutions=substitutions)
            use_copyright = False
        configuration_source.readfile(self.files_per_i2c_type[i2c_type]['configuration_source'],
                                      make_static=False, use_copyright=use_copyright)
        configuration_source.add_project_include(self.driver_configuration_header)
        configuration_source_destination = os.path.basename(self.files_per_i2c_type[i2c_type]['configuration_source'])

        # Generate example source file
        for file_name in self.example_source_files:
            example_source = SourceSquash(print_file_separator=False, version_tag=version_tag)
            substitutions = None
            if self.other_substitutions_per_sensor.has_key(sensor):
                substitutions = self.other_substitutions_per_sensor[sensor]
            use_copyright = True
            if self.copyright_header_path:
                example_source.readfile(self.copyright_header_path, use_copyright=True, substitutions=substitutions)
                use_copyright = False
            example_source.readfile(file_name, make_static=False,
                                    use_copyright=use_copyright)
            example_source.add_project_include(driver_header_destination)
            dst = os.path.basename(file_name)
            example_source.write(os.path.join(output_directory, dst))

        # Copy any other files
        for file_name in self.copy_files:
            copy_file = SourceSquash(print_file_separator=False, version_tag=version_tag)
            if file_name.endswith('.h'):
                # Add current .h file to driver header
                driver_header.add_project_include(os.path.basename(file_name))
                # Add driver_configuration_header to any copy_file
                # Most copy files depend on our equivalent of the stdint typedefs
                copy_file.add_project_include(self.driver_configuration_header)
            elif file_name.endswith('.c'):  # Make sure that any .c-file includes its .h-file, if available
                begin_str = file_name[:-2]
                header = "{0}{1}".format(begin_str, '.h')
                if header in self.copy_files:
                    copy_file.add_project_include(os.path.basename(header))

            substitutions = None
            if self.other_substitutions_per_sensor.has_key(sensor):
                substitutions = self.other_substitutions_per_sensor[sensor]
            use_copyright = True
            if self.copyright_header_path:
                copy_file.readfile(self.copyright_header_path, use_copyright=True, substitutions=substitutions)
                use_copyright = False
            copy_file.readfile(file_name, make_static=False, use_copyright=use_copyright)
            dst = os.path.basename(file_name)
            copy_file.write(os.path.join(output_directory, dst))

        driver_header.write(os.path.join(output_directory, driver_header_destination))
        driver_source.write(os.path.join(output_directory, driver_source_destination))
        configuration_header.write(os.path.join(output_directory, self.driver_configuration_header))
        configuration_source.write(os.path.join(output_directory, configuration_source_destination))

    def get_git_tag(self):
        try:
            sht_repo = git.repo.Repo()
            tag = sht_repo.git.describe(['--dirty', '--always'])
            tag_split = tag.split('-')
            if len(tag_split) == 3:
                tag_split.append(u'')
            if len(tag_split) > 1 and (
                    (tag_split[1] != u'0') or (tag_split[3] == u'dirty')):
                return tag
            else:
                return tag_split[0]
        except git.exc.InvalidGitRepositoryError:
            return EMPTY_TAG_STRING

    def get_version_tag(self):
        if os.path.isfile(RELEASE_TAG_FILE):
            f = open(RELEASE_TAG_FILE, 'r')
            return f.readline().strip('\n')
        elif use_git:
            return self.get_git_tag()
        else:
            return EMPTY_TAG_STRING


def main(args):
    # Instantiate SHTDriverRelease
    sht_driver_release = SHTDriverRelease()

    all_sensor_types = sht_driver_release.merge_file_per_sensor.keys()
    all_i2c_types = sht_driver_release.files_per_i2c_type.keys()

    parser = argparse.ArgumentParser(description='Release Generic Driver')
    parser.add_argument('--sensor', type=str, choices=['all'] + all_sensor_types, default='all')
    parser.add_argument('--i2c-type', type=str, choices=['all'] + all_i2c_types, default='all')
    parser.add_argument('output_directory', type=str, default='release', nargs='?')
    namespace = parser.parse_args(args)

    output_directory = os.path.abspath(namespace.output_directory)
    os.chdir(os.path.dirname(__file__))

    if namespace.sensor == 'all':
        sensor_types = all_sensor_types
    else:
        sensor_types = [namespace.sensor]

    if namespace.i2c_type == 'all':
        i2c_types = all_i2c_types
    else:
        i2c_types = [namespace.i2c_type]

    for i2c_type in i2c_types:
        for sensor_type in sensor_types:
            directory = os.path.join(output_directory, '{0}_{1}'.format(sensor_type, i2c_type))
            sht_driver_release.release_configuration(sensor_type, i2c_type, directory)


if __name__ == '__main__':
    main(sys.argv[1:])
