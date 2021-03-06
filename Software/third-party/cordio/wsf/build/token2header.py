###################################################################################################
#
# Token trace header file generator
#
# Copyright (c) 2016-2019 Arm Ltd. All Rights Reserved.
#
# Copyright (c) 2019 Packetcraft, Inc.
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
#
###################################################################################################

import sys
import os
import csv
import errno

# -------------------------------------------------------------------------------------------------
#     Constants
# -------------------------------------------------------------------------------------------------

[ID, LINE, FILE, SUBSYS, STATUS, MSG] = range(6)

FILE_HDR = \
r'''/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief      Token trace definitions.
 *
 *  Copyright (c) 2019 Packetcraft, Inc. 
 * 
 *  Copyright (c) 2022 Delft University of Technology.
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/
'''

# -------------------------------------------------------------------------------------------------
#     Functions
# -------------------------------------------------------------------------------------------------

## Reverse value
#
def Reverse(val, count):
    temp = 0

    while count:
        temp <<= 1
        temp += val & 1
        val >>= 1
        count = count - 1

    return temp

## Get parameter string mask
#
def GetParamStringMask(msg):
    mark = False
    pos = 0
    mask = 0

    for c in msg:
        if mark:
            if c == 's':
                mask = mask | (1 << pos)
            pos = pos + 1

        mark = True if (c == '%') else False

    return Reverse(mask, pos)

## Read and parse token file
#
def ReadTokenFile(in_file):
    with open(in_file, 'r') as f:
        text = f.read()

    # Split input file into 2-D array
    tokens = csv.reader(text.split('\n'), skipinitialspace=True)
    tokens = filter(None, tokens)

    token_file_line_no = 0

    # Modify token data
    for f in tokens:
        token_file_line_no += 1

        if len(f) != 6:
            print >> sys.stderr, 'warning: %s:%u: invalid token definition format' % (in_file, token_file_line_no)
            continue

        # Replace the module and line with the token ID
        line = int(f[LINE])
        module_id = int(f[ID], 16)
        token = (line << 16) | module_id
        f[ID] = token

        # Remove leading "../../../.." from filenames to save ROM
        f[FILE] = f[FILE].lstrip('.//')

    # Sort tokens on the token ID
    tokens = sorted(tokens, key=lambda x: x[0])

    return tokens

## Generate token header definition file
#
def GenerateHeader(tokens):
    out_text  = FILE_HDR + '\n'
    out_text += '/* Generated by %s. */\n' % os.path.basename(sys.argv[0])
    out_text += '\n'
    out_text += '#ifndef TOKENS_H\n'
    out_text += '#define TOKENS_H\n'
    out_text += '\n'
    out_text += 'typedef struct\n'
    out_text += '{\n'
    out_text += '  uint32_t    token;\n'
    out_text += '  uint8_t     paramInfo;\n'
    out_text += '  const char  *pSubsys;\n'
    out_text += '  const char  *pStatus;\n'
    out_text += '  const char  *pMsg;\n'
    out_text += '} tokenElem_t;\n'
    out_text += '\n'
    out_text += 'typedef struct\n'
    out_text += '{\n'
    out_text += '  uint32_t    strId;\n'
    out_text += '  const char  *pText;\n'
    out_text += '} tokenStr_t;\n'
    out_text += '\n'

    # Output the token elements
    line_no = 0
    out_text += 'static const tokenElem_t tokenLookupTbl[] =\n'
    out_text += '{\n'
    for f in tokens:
        if not f:
            continue

        if f[LINE] == '0':
            continue

        if line_no != 0:
            out_text += ',\n  { '
        else:
            out_text += '  { '

        if len(f) != 6:
            continue

        numParams = f[MSG].count('%')
        paramStrMask = GetParamStringMask(f[MSG])
        paramInfo = (paramStrMask << 4) | numParams

        out_text += '0x%08x' % f[ID]
        out_text += ', 0x%02x' % paramInfo
        out_text += ', "%s"' % f[SUBSYS].strip("'")
        out_text += ', "%s"' % f[STATUS].strip("'")
        out_text += ', "%s"' % f[MSG].strip("'")
        out_text += ' }'

        line_no += 1

    out_text += '\n};\n\n'

    # Output the string table elements
    line_no = 0
    out_text += 'static const tokenStr_t tokenStrTbl[] =\n'
    out_text += '{\n'
    for f in tokens:
        if not f:
            continue

        if f[LINE] != '0':
            continue

        if line_no != 0:
            out_text += ',\n  { '
        else:
            out_text += '  { '

        numParams = f[MSG].count('%')

        out_text += '0x%04x' % f[ID]
        out_text += ', "%s"' % f[FILE].strip('"')
        out_text += ' }'

        line_no += 1

    out_text += '\n};\n\n'

    out_text += '#endif /* TOKENS_H */\n'

    return out_text

## Write output text to file
#
def WriteHeader(out_file, out_text):
    with open(out_file, 'w') as f:
        f.write(out_text)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print >> sys.stderr, 'error: missing arguments'
        print 'Usage: %s token_file output_file' % os.path.basename(sys.argv[0])
        exit(errno.EINVAL)

    in_file = sys.argv[1]
    out_file = sys.argv[2]

    tokens = ReadTokenFile(in_file)
    out_text = GenerateHeader(tokens)
    WriteHeader(out_file, out_text)
