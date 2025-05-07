
import os
import glob

from xml.etree import ElementTree

root_path = os.path.dirname(os.path.dirname(__file__))

messages_path = os.path.join(root_path, 'messages')
xml_filename_list = glob.glob(os.path.join(messages_path, '*_messages.xml'))

source_path = os.path.join(root_path, 'include/mdsplusplus')
output_filename = os.path.join(source_path, 'Exceptions.hpp')

SEVERITY_MAP = {
    'warning': 0,
    'success': 1,
    'error': 2,
    'info': 3,
    'fatal': 4,
    'internal': 7
}

SEVERITY_CODES = ['W', 'S', 'E', 'I', 'F', '?', '?', '?']

output_file = open(output_filename, 'wt')
output_file.write('''#ifndef MDSPLUS_EXCEPTIONS_HPP
#define MDSPLUS_EXCEPTIONS_HPP

#include <stdexcept>

#include <mdsshr.h>
#include <camshr_messages.h>
#include <mdsdcl_messages.h>
#include <mdsshr_messages.h>
#include <mitdevices_messages.h>
#include <servershr_messages.h>
#include <tcl_messages.h>
#include <tdishr_messages.h>
#include <treeshr_messages.h>

namespace mdsplus {

class MDSplusException : public std::runtime_error
{
public:

    inline MDSplusException()
        : std::runtime_error("Unknown Status Code")
    { }

    inline MDSplusException(int status)
        : std::runtime_error(MdsGetMsg(status))
        , _status(status)
    { }

    inline int getStatus() const {
        return _status;
    }

private:

    int _status;

}; // class MdsException

''')

mapping = {}

for xml_filename in xml_filename_list:
    with open(xml_filename, 'rt') as xml_file:
        try:
            root = ElementTree.parse(xml_filename).getroot()
            for facility in root.iter('facility'):
                facility_name = facility.get('name')
                facility_value = int(facility.get('value'))

                if facility_name != 'MDSplus':
                    output_file.write(f'class {facility_name}Exception : public MDSplusException\n')
                    output_file.write(f'{{\n')
                    output_file.write(f'public:\n')
                    output_file.write(f'\n')
                    output_file.write(f'    inline {facility_name}Exception(int status)\n')
                    output_file.write(f'        : MDSplusException(status)\n')
                    output_file.write(f'    {{ }}\n')
                    output_file.write(f'\n')
                    output_file.write(f'}}; // {facility_name}Exception\n')
                    output_file.write(f'\n')

                for status in facility.iter('status'):
                    status_name = status.get('name')
                    status_value = int(status.get('value'))
                    nice_name = status.get('nicename')

                    if nice_name is None:
                        continue

                    if status_name.startswith('CAM_') or status_name == 'CAMACERR':
                        continue

                    facility_name_override = status.get('facnam')
                    facility_abbreviation = status.get('facabb', facility_name)
                    if facility_name_override is not None:
                        continue

                    deprecated = status.get('deprecated', None)
                    if deprecated is not None:
                        continue

                    severity = SEVERITY_MAP[status.get('severity').lower()]

                    message = status.get('text', None)
                    if message is None:
                        continue

                    message = message.replace('  ', ' ')
                    message = message[0].upper() + message[ 1 : ]
                    if message[-1] == '.':
                        message = message[ : -1 ]

                    class_name = facility_name + nice_name
                    message_name = facility_name + status_name
                    message_value = (facility_value << 16) + (status_value << 3) + severity
                    message_prefix = f'%{facility_name.upper()}-{SEVERITY_CODES[severity]}-{status_name.upper()}'

                    mapping[message_name] = class_name

                    output_file.write(f'class {class_name} : public {facility_name}Exception\n')
                    output_file.write(f'{{\n')
                    output_file.write(f'public:\n')
                    output_file.write(f'\n')
                    output_file.write(f'    inline {class_name}()\n')
                    output_file.write(f'        : {facility_name}Exception({message_name})\n')
                    output_file.write(f'    {{ }}\n')
                    output_file.write(f'\n')
                    output_file.write(f'}}; // {class_name}\n')
                    output_file.write(f'\n')

        except Exception as e:
            print(e)

output_file.write('''
inline void throwException(int status)
{
    switch (status)
    {''')

for message_name, class_name in mapping.items():
    output_file.write('\n')
    output_file.write(f'    case {message_name}:\n')
    output_file.write(f'        throw {class_name}();\n')

output_file.write('''
    }
}

} // namespace mdsplus

#endif // MDSPLUS_EXCEPTIONS_HPP
''')