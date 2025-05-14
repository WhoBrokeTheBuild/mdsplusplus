#!/usr/bin/env python3

import os

root_path = os.path.dirname(os.path.dirname(__file__))

includes = set()
macros = {}
sections = {}

def process_macro(macro, line):
    global macros

    leading_spaces = line.split('_')[0]
    args = line.split('(')[1].split(')')[0].split(', ')
    substitutions = dict(zip(macro['args'], args))

    lines = []
    for line in macro['lines']:
        for sub_key, sub_value in substitutions.items():
            line = line.replace(f'#{sub_key}', f'"{sub_value}"')
            line = line.replace(sub_key, sub_value)

        found_macro = False
        for name, inner_macro in macros.items():
            if f'{name}(' in line:
                lines.extend(process_macro(inner_macro, line))
                found_macro = True
                break

        if not found_macro:
            line = leading_spaces + macro['indent'] + line
            lines.append(line.rstrip())

    if macro['tail'] is not None:
        lines.append('')
        lines.append(macro['indent'] + macro['tail'])

    return lines

def process_header(filename):
    global includes, macros, sections

    current_macro = None
    macro_condition = None
    macro_leading_spaces = ''

    current_section = None

    with open(filename, 'rt') as input_file:
        lines = input_file.readlines()
        for line in lines:

            if 'MDSPLUS_' in line and '_HPP' in line:
                continue

            if line.lstrip().startswith('#if') or line.startswith('#else'):
                macro_condition = line.strip()
            elif line.lstrip().startswith('#endif'):
                macro_condition = None

            if line.startswith('namespace'):
                current_section = line.split()[1]
                if current_section not in sections:
                    sections[current_section] = []
            
            elif line.startswith('} // namespace'):
                current_section = None

            elif line.startswith('extern "C"'):
                current_section = 'extern "C"'
                if current_section not in sections:
                    sections[current_section] = []
            
            elif line.startswith('} // extern "C"'):
                current_section = None

            elif line.lstrip().startswith('#define _MDSPLUS_'):

                definition = line.split(maxsplit=1)[1].rstrip()
                name, args = definition.split('(', maxsplit=1)
                args = args.removesuffix('\\').rstrip().removesuffix(')').split(', ')

                macro_leading_spaces = line.split('#')[0] + '    '
                current_macro = name

                indent = ''
                tail = ''

                if macro_condition is not None:
                    indent += '    '

                    if macro_condition.startswith('#if'):
                        tail = '#endif'

                if current_macro not in macros:
                    macros[current_macro] = {
                        'args': args,
                        'tail': tail,
                        'lines': [],
                        'indent': indent,
                    }

                if macro_condition is not None:
                    macros[current_macro]['lines'].append('')
                    macros[current_macro]['lines'].append(macro_condition)
                    macros[current_macro]['lines'].append('')
                
            elif line.lstrip().startswith('#include <mdsplusplus/'):
                include_filename = line.rstrip('>\n').split('<')[1]
                include_filename = os.path.join(root_path, 'source', include_filename)
                process_header(include_filename)
            
            elif line.lstrip().startswith('#include <'):
                include_filename = line.rstrip('>\n').split('<')[1]
                includes.add(include_filename)

            elif line.startswith('#include "'):
                pass

            else:
                if '///' in line:
                    line = line.split('///')[0]

                if current_macro is not None:
                    real_line = line.removeprefix(macro_leading_spaces).removesuffix('\\\n').rstrip()
                    macros[current_macro]['lines'].append(macros[current_macro]['indent'] + real_line)
                    if not line.endswith('\\\n'):
                        current_macro = None
                    
                    continue
                
                if current_section is not None:
                    found_macro = False
                    for name, macro in macros.items():
                        if f'{name}(' in line:
                            sections[current_section].extend(process_macro(macro, line))
                            found_macro = True
                            break
                    
                    if not found_macro:
                        sections[current_section].append(line.rstrip())

input_filename = os.path.join(root_path, 'source/mdsplus.hpp')
process_header(input_filename)

output_file = open('include/mdsplus.hpp', 'wt')

output_file.write('''//
// Copyright (c) 2025, Massachusetts Institute of Technology All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the “Software”), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
''')
output_file.write('\n')

output_file.write('#ifndef MDSPLUS_HPP\n')
output_file.write('#define MDSPLUS_HPP\n')
output_file.write('\n')

## C++ Includes

includes.remove('optional')
includes.remove('span')

for include in sorted(includes):
    if '.h' not in include:
        output_file.write(f'#include <{include}>\n')
output_file.write('\n')

output_file.write('#if __has_include(<optional>)\n')
output_file.write('    #include <optional>\n')
output_file.write('#endif\n')
output_file.write('\n')

output_file.write('#if __has_include(<span>)\n')
output_file.write('    #include <span>\n')
output_file.write('#endif\n')
output_file.write('\n')

## C Includes

output_file.write('extern "C" {\n')
output_file.write('\n')

for include in sorted(includes):
    if '.h' in include:
        output_file.write(f'    #include <{include}>\n')
output_file.write('\n')

last_line = ''
for line in sections['extern "C"']:
    if len(line) == len(last_line) == 0:
        continue

    output_file.write(f'{line}\n')
    last_line = line

del sections['extern "C"']

output_file.write('} // extern "C"\n')
output_file.write('\n')

## Namespaces & Code

for namespace, lines in sections.items():
    output_file.write(f'namespace {namespace} {{\n')
    output_file.write('\n')

    last_line = ''
    for line in lines:
        if len(line) == len(last_line) == 0:
            continue

        output_file.write(f'{line}\n')
        last_line = line

    output_file.write(f'}} // namespace {namespace}\n')
    output_file.write('\n')
    

output_file.write('#endif // MDSPLUS_HPP\n')
output_file.close()