#!/usr/bin/python3

import sys

TYPES = ['void', 'bool',
         'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t',
         'int8_t', 'int16_t', 'int32_t', 'int64_t',
         'UtObject']

NUMBER_CHARACTERS = '0123456789'
HEX_NUMBER_CHARACTERS = '0123456789abcdefABCDEF'
KEYWORD_CHARACTERS = 'abcdefghijklmnopqrstuvwxyz' \
                     'ABCDEFGHIJKLMNOPQRSTUVWXYZ' \
                     '01234567890_'
SINGLE_TOKEN_CHARACTERS = '#*(){}[];,=.'
OPERATORS = ['>>', '<<']


class Comment:
    def __init__(self, text):
        self.text = text

    def __repr__(self):
        return 'Comment(' + repr(self.text) + ')'


class Include:
    def __init__(self, path, is_system):
        self.path = path
        self.is_system = is_system

    def __repr__(self):
        return 'Include(' + repr(self.path) + ', ' + repr(self.is_system) + ')'


class Define:
    def __init__(self, name, args, tokens):
        self.name = name
        self.args = args
        self.tokens = tokens

    def __repr__(self):
        return 'Define(' + \
            repr(self.name) + ', ' + \
            repr(self.args) + ', ' + \
            repr(self.tokens) + ')'


class Pragma:
    def __init__(self, tokens):
        self.tokens = tokens

    def __repr__(self):
        return 'Pragma(' + repr(self.tokens) + ')'


class Type:
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return 'Type(' + repr(self.name) + ')'


class Enum:
    def __init__(self, values):
        self.values = values

    def __repr__(self):
        return 'Enum(' + repr(self.values) + ')'


class Struct:
    def __init__(self, members):
        self.members = members

    def __repr__(self):
        return 'Struct(' + repr(self.members) + ')'


class Typedef:
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def __repr__(self):
        return 'Typedef(' + repr(self.name) + ', ' + repr(self.value) + ')'


class FunctionDefinition:
    def __init__(self, name, return_type, args, variable_args):
        self.name = name
        self.return_type = return_type
        self.args = args
        self.variable_args = variable_args

    def has_arg(self, name):
        for (_, arg_name) in self.args:
            if arg_name == name:
                return True
        return False

    def __repr__(self):
        return 'FunctionDefinition(' + \
               repr(self.name) + \
               ', ' + \
               repr(self.return_type) + \
               ', ' + \
               repr(self.args) + \
               ', ' + \
               repr(self.variable_args) + \
               ')'


class VariableDefinition:
    def __init__(self, name, type_):
        self.name = name
        self.type_ = type_

    def __repr__(self):
        return 'VariableDefinition(' + \
               repr(self.name) + \
               ', ' + \
               repr(self.type_) + \
               ')'


class FunctionPointer:
    def __init__(self, return_type, args, variable_args):
        self.return_type = return_type
        self.args = args
        self.variable_args = variable_args

    def __repr__(self):
        return 'FunctionPointer(' + \
               repr(self.return_type) + \
               ', ' + \
               repr(self.args) + \
               ', ' + \
               repr(self.variable_args) + \
               ')'


class FunctionCall:
    def __init__(self, name, values):
        self.name = name
        self.values = values

    def __repr__(self):
        return 'FunctionCall(' + \
               repr(self.name) + \
               ', ' + \
               repr(self.values) + \
               ')'


def tokenize_c_comment(text):
    assert text.startswith('/*')
    end = 2
    while end < len(text):
        if text.startswith('*/', end):
            return (text[:end], text[end:])
        end += 1
    raise Exception('Unterminated comment')


def tokenize_cpp_comment(text):
    assert text.startswith('//')
    end = 2
    while end < len(text) and text[end] != '\n':
        end += 1
    return (text[:end], text[end:])


def tokenize_hex_number(text):
    assert text.startswith('0x')
    end = 2
    while end < len(text) and text[end] in HEX_NUMBER_CHARACTERS:
        end += 1
    return (text[:end], text[end:])


def tokenize_number(text):
    assert len(text) > 0 and text[0] in NUMBER_CHARACTERS
    end = 0
    while end < len(text) and text[end] in NUMBER_CHARACTERS:
        end += 1
    return (text[:end], text[end:])


def tokenize_keyword(text):
    end = 0
    while end < len(text) and text[end] in KEYWORD_CHARACTERS:
        end += 1
    return (text[:end], text[end:])


def tokenize_string(text):
    assert text.startswith('"')
    end = 1
    in_escape = False
    while end < len(text) and (in_escape or text[end] != '"'):
        if in_escape:
            in_escape = False
        elif text[end] == '\\':
            in_escape = True
        end += 1
    return (text[0:end + 1], text[end+1:])


def tokenize_system_include(text):
    assert text.startswith('<')
    end = 1
    while end < len(text) and text[end] != '>':
        end += 1
    return (text[:end + 1], text[end+1:])


def tokenize(text):
    tokens = []
    in_preprocessor = False
    while len(text) > 0:
        if in_preprocessor and text[0] == '\n':
            in_preprocessor = False
            token = text[0]
            text = text[1:]
        elif not in_preprocessor and text[0] == '#':
            in_preprocessor = True
            token = text[0]
            text = text[1:]
        elif text[0] in ' \r\n\t':
            text = text[1:]
            continue
        elif text.startswith('\\\n'):
            text = text[2:]
            continue
        elif text.startswith('/*'):
            (token, text) = tokenize_c_comment(text)
        elif text.startswith('//'):
            (token, text) = tokenize_cpp_comment(text)
        elif text.startswith('<<') or text.startswith('>>'):
            token = text[:2]
            text = text[2:]
        elif text.startswith('0x'):
            (token, text) = tokenize_hex_number(text)
        elif text[0] in SINGLE_TOKEN_CHARACTERS:
            token = text[0]
            text = text[1:]
        elif text[0] in NUMBER_CHARACTERS:
            (token, text) = tokenize_number(text)
        elif text[0] in KEYWORD_CHARACTERS:
            (token, text) = tokenize_keyword(text)
        elif text[0] == '"':
            (token, text) = tokenize_string(text)
        elif text[0] == '<':
            (token, text) = tokenize_system_include(text)
        else:
            raise Exception('Unknown character ' + repr(text[0]))
        tokens.append(token)

    return tokens


def assert_token(tokens, value):
    if len(tokens) == 0:
        raise Exception('File end when expecting ' + value)
    if tokens[0] != value:
        raise Exception('Expected ' + repr(value) + ', got ' + repr(tokens[0]))
    return tokens[1:]


def has_token(tokens, value):
    if len(tokens) > 0 and tokens[0] == value:
        return (True, tokens[1:])
    else:
        return (False, tokens)


def parse_string(tokens):
    if len(tokens) == 0:
        raise Exception('File end when expecting string')
    token = tokens[0]
    if not (token.startswith('"') and token.endswith('"')):
        raise Exception('Expected string, got ' + repr(token))
    tokens = tokens[1:]
    value = ''
    in_escape = False
    for c in token[1:-1]:
        if in_escape:
            if c == 'r':
                value += '\r'
            elif c == 'n':
                value += '\n'
            elif c == 't':
                value += '\t'
            elif c == '\\':
                value += '\\'
            elif c == '"':
                value += '"'
            else:
                raise Exception('Unknown escape character ' + c)
        elif c == '\\':
            in_escape = True
        else:
            value += c
    return (value, tokens)


def parse_system_include(tokens):
    if len(tokens) == 0:
        raise Exception('File end when expecting system include')
    token = tokens[0]
    if not (token.startswith('<') and token.endswith('>')):
        raise Exception('Expected system include, got ' + repr(token))
    tokens = tokens[1:]
    path = token[1:-1]
    return (path, tokens)


def parse_include(tokens):
    tokens = assert_token(tokens, 'include')
    if len(tokens) == 0:
        raise Exception('Missing include path')
    is_system = False
    if tokens[0].startswith('"'):
        (path, tokens) = parse_string(tokens)
    elif tokens[0].startswith('<'):
        is_system = True
        (path, tokens) = parse_system_include(tokens)
    else:
        raise Exception('Invalid include value ' + tokens[0])
    tokens = assert_token(tokens, '\n')
    return (Include(path, is_system), tokens)


def parse_preprocessor_tokens(tokens):
    preprocessor_tokens = []
    while len(tokens) > 0:
        if (tokens[0] == '\n'):
            tokens = tokens[1:]
            break
        preprocessor_tokens.append(tokens[0])
        tokens = tokens[1:]
    return (preprocessor_tokens, tokens)


def parse_define_args(tokens):
    args = []
    while len(tokens) > 0:
        (done, tokens) = has_token(tokens, ')')
        if done:
            return (args, tokens)

        if len(args) > 0:
            tokens = assert_token(tokens, ',')

        (name, tokens) = parse_name(tokens)
        args.append(name)
    raise Exception('Unterminated args')


def parse_define(tokens):
    tokens = assert_token(tokens, 'define')
    (name, tokens) = parse_name(tokens)
    (has_args, tokens) = has_token(tokens, '(')
    if has_args:
        (args, tokens) = parse_define_args(tokens)
    else:
        args = []
    (define_tokens, tokens) = parse_preprocessor_tokens(tokens)
    return (Define(name, args, define_tokens), tokens)


def parse_pragma(tokens):
    tokens = assert_token(tokens, 'pragma')
    (pragma_tokens, tokens) = parse_preprocessor_tokens(tokens)
    return (Pragma(pragma_tokens), tokens)


def parse_preprocessor(tokens):
    tokens = assert_token(tokens, '#')
    if len(tokens) == 0:
        raise Exception('File end when expecting preprocessor directive')
    keyword = tokens[0]
    if keyword == 'include':
        return parse_include(tokens)
    elif keyword == 'define':
        return parse_define(tokens)
    elif keyword == 'pragma':
        return parse_pragma(tokens)
    else:
        raise Exception('Unknown preprocessor keyword ' + repr(keyword))


def parse_comment(tokens):
    if len(tokens) == 0 or not tokens[0].startswith('//'):
        return (None, tokens)
    token = tokens[0]
    tokens = tokens[1:]
    return (Comment(token[2:]), tokens)


def parse_name(tokens):
    assert len(tokens) > 0
    name = tokens[0]
    for c in name:
        if c not in KEYWORD_CHARACTERS:
            raise Exception('Expected name, got ' + repr(name))
    return (name, tokens[1:])


def parse_number(tokens):
    assert len(tokens) > 0
    assert tokens[0][0] in NUMBER_CHARACTERS
    return (tokens[0], tokens[1:])


def parse_line_end(tokens):
    return assert_token(tokens, ';')


def parse_enum(tokens):
    tokens = assert_token(tokens, 'enum')
    tokens = assert_token(tokens, '{')
    values = []
    while len(tokens) > 0:
        (_, tokens) = parse_comment(tokens)

        (done, tokens) = has_token(tokens, '}')
        if done:
            return (Enum(values), None, tokens)

        if len(values) > 0:
            tokens = assert_token(tokens, ',')

        (name, tokens) = parse_name(tokens)
        value = None
        if len(tokens) > 0 and tokens[0] == '=':
            tokens = assert_token(tokens, '=')
            (value, tokens) = parse_number(tokens)
            if len(tokens) > 0 and tokens[0] in OPERATORS:
                operator = tokens[0]
                tokens = tokens[1:]
                (value2, tokens) = parse_number(tokens)
                value = value + ' ' + operator + ' ' + value2
        values.append((name, value))
    raise Exception('Unterminated enum')


def parse_struct(tokens):
    tokens = assert_token(tokens, 'struct')
    (is_unnamed, tokens) = has_token(tokens, '{')
    name = None
    if not is_unnamed:
        (name, tokens) = parse_name(tokens)
        (is_defined, tokens) = has_token(tokens, '{')
        if not is_defined:
            return (Type('struct ' + name), None, tokens)
    members = []
    while len(tokens) > 0:
        (_, tokens) = parse_comment(tokens)

        (done, tokens) = has_token(tokens, '}')
        if done:
            return (Struct(members), None, tokens)

        (type_, name, tokens) = parse_type(tokens)
        if name is None:
            (name, tokens) = parse_name(tokens)
        (is_array, tokens) = has_token(tokens, '[')
        if is_array:
            tokens = assert_token(tokens, ']')
        tokens = parse_line_end(tokens)

        members.append((type_, name))
    raise Exception('Unterminated struct')


def parse_function_pointer(tokens):
    tokens = assert_token(tokens, '(')
    tokens = assert_token(tokens, '*')
    (name, tokens) = parse_name(tokens)
    tokens = assert_token(tokens, ')')
    tokens = assert_token(tokens, '(')
    (args, variable_args, tokens) = parse_args(tokens)
    return (name, args, variable_args, tokens)


def parse_type(tokens):
    assert len(tokens) > 0
    if tokens[0] == 'enum':
        return parse_enum(tokens)
    elif tokens[0] == 'struct':
        return parse_struct(tokens)
    (_, tokens) = has_token(tokens, 'static')
    (_, tokens) = has_token(tokens, 'inline')
    (is_extern, tokens) = has_token(tokens, 'extern')
    (is_const, tokens) = has_token(tokens, 'const')
    (is_unsigned, tokens) = has_token(tokens, 'unsigned')
    (type_name, tokens) = parse_name(tokens)
    if is_extern:
        type_name = 'extern ' + type_name
    if is_const:
        type_name = 'const ' + type_name
    if is_unsigned:
        type_name = 'unsigned ' + type_name
    while True:
        (has_pointer, tokens) = has_token(tokens, '*')
        if not has_pointer:
            break
        type_name += '*'

    if len(tokens) >= 2 and tokens[0] == '(' and tokens[1] == '*':
        (name, args, variable_args, tokens) = parse_function_pointer(tokens)
        return (FunctionPointer(type_name, args, variable_args), name, tokens)

    return (Type(type_name), None, tokens)


def parse_typedef(tokens):
    tokens = assert_token(tokens, 'typedef')
    (type_, name, tokens) = parse_type(tokens)
    if name is None:
        (name, tokens) = parse_name(tokens)
    tokens = parse_line_end(tokens)
    return (Typedef(name, type_), tokens)


def parse_variable_args(tokens):
    if len(tokens) < 3 or tokens[:3] != ['.', '.', '.']:
        return (False, tokens)
    else:
        return (True, tokens[3:])


def parse_args(tokens):
    args = []
    while len(tokens) > 0:
        (done, tokens) = has_token(tokens, ')')
        if done:
            return (args, False, tokens)

        if len(args) > 0:
            tokens = assert_token(tokens, ',')

        (has_variable_args, tokens) = parse_variable_args(tokens)
        if has_variable_args:
            args.append(('...', ''))
            tokens = assert_token(tokens, ')')
            return (args, True, tokens)

        (arg_type, name, tokens) = parse_type(tokens)
        if name is None:
            (name, tokens) = parse_name(tokens)
        args.append((arg_type, name))
    raise Exception('Unterminated args')


def parse_attribute_args(tokens):
    tokens = assert_token(tokens, '(')
    tokens = assert_token(tokens, '(')
    (_, tokens) = parse_name(tokens)
    tokens = assert_token(tokens, '(')
    args = []
    while len(tokens) > 0:
        (done, tokens) = has_token(tokens, ')')
        if done:
            break
        if len(args) > 0:
            tokens = assert_token(tokens, ',')
        args.append(tokens[0])
        tokens = tokens[1:]
    tokens = assert_token(tokens, ')')
    tokens = assert_token(tokens, ')')
    return tokens


def skip_contents(tokens):
    count = 1
    while len(tokens) > 0:
        token = tokens[0]
        tokens = tokens[1:]
        if token == '{':
            count += 1
        elif token == '}':
            count -= 1
            if count == 0:
                return tokens
    raise Exception('Unterminated function content')


def parse_function_or_variable_definition(tokens):
    (return_type, _, tokens) = parse_type(tokens)
    (done, tokens) = has_token(tokens, ';')
    if done:
        return (return_type, tokens)
    (name, tokens) = parse_name(tokens)
    (has_args, tokens) = has_token(tokens, '(')
    has_contents = False
    if has_args:
        (args, variable_args, tokens) = parse_args(tokens)
        (has_contents, tokens) = has_token(tokens, '{')
        if has_contents:
            tokens = skip_contents(tokens)
    (has_attribute, tokens) = has_token(tokens, '__attribute__')
    if has_attribute:
        tokens = parse_attribute_args(tokens)
    if not has_contents:
        tokens = parse_line_end(tokens)
    if has_args:
        return (FunctionDefinition(name, return_type, args, variable_args),
                tokens)
    else:
        return (VariableDefinition(name, return_type), tokens)


def parse_header(filename):
    text = open(filename).read()
    tokens = tokenize(text)
    statements = []
    while len(tokens) > 0:
        token = tokens[0]
        if token == '#':
            (statement, tokens) = parse_preprocessor(tokens)
        elif token.startswith('//'):
            (statement, tokens) = parse_comment(tokens)
        elif token == 'typedef':
            (statement, tokens) = parse_typedef(tokens)
        elif token[0] in KEYWORD_CHARACTERS:
            (statement, tokens) = parse_function_or_variable_definition(tokens)
        else:
            raise Exception('Unknown token: ' + token)
        statements.append(statement)
    return statements


class ApiFunction:
    def __init__(self, function, tags, comments):
        self.function = function
        self.tags = tags
        self.comments = comments


class ApiEnum:
    def __init__(self, name, enum, comments):
        self.name = name
        self.enum = enum
        self.comments = comments


def validate_tag(tag):
    name = tag[0]
    if name == 'return-type':
        return len(tag) >= 2
    elif name == 'return-ref':
        return len(tag) == 1
    elif name == 'return-element-type':
        return len(tag) >= 2
    elif name == 'take-ref':
        return len(tag) == 2
    elif name == 'arg-type':
        return len(tag) >= 3
    else:
        return False


source_dir = '../src'
statements = parse_header(source_dir + '/ut.h')
functions = []
enums = []
for statement in statements:
    if isinstance(statement, Include):
        comments = []
        tags = []
        for s in parse_header(source_dir + '/' + statement.path):
            if isinstance(s, Comment):
                if s.text.startswith('/'):
                    text = s.text[1:]
                    if text.startswith(' '):
                        text = text[1:]
                    if text.startswith('!'):
                        tag = text[1:].split()
                        if not validate_tag(tag):
                            print('Invalid tag %s' % tag)
                        tags.append(tag)
                    else:
                        comments.append(text)
                else:
                    comments = []
                    tags = []

            if isinstance(s, FunctionDefinition) and \
               not s.name.startswith('_'):
                functions.append(ApiFunction(s, tags, comments))
            elif isinstance(s, Define) and len(s.args) > 0:
                functions.append(ApiFunction(s, tags, comments))
            elif isinstance(s, Typedef) and isinstance(s.value, Enum):
                enums.append(ApiEnum(s.name, s.value, comments))

            if not isinstance(s, Comment):
                comments = []
                tags = []


def make_doc(comments):
    doc = ''
    for c in comments:
        doc += c + '\n'
    while doc.endswith('\n'):
        doc = doc[:-1]
    return doc


def extract_links(doc):
    links = []
    link = None
    for c in doc:
        if c == '[':
            link = ''
        elif link is not None:
            if c == ']':
                links.append(link)
                link = None
            else:
                link += c

    assert link is None

    return links


def get_return_type(tags):
    for tag in tags:
        if tag[0] == 'return-type':
            return tag[1:]
    return None


def has_return_ref(tags):
    for tag in tags:
        if tag[0] == 'return-ref':
            return True
    return False


def get_arg_type(tags, name):
    prefix = 'arg-type %s ' % name
    for tag in tags:
        if tag[0] == 'arg-type' and tag[1] == name:
            return tag[2:]
    return None


doc_text = ''
functions.sort(key=lambda x: x.function.name)
undocumented_functions = []
undocumented_function_args = []
undocumented_function_arg_types = []
invalid_function_arg_types = []
undocumented_function_return_types = []
invalid_function_return_types = []
for function in functions:
    doc_text += '\n'
    doc_text += function.function.name + '\n'
    doc = make_doc(function.comments)
    doc_text += doc + '\n'
    if len(function.comments) == 0:
        undocumented_functions.append(function)
        continue

    links = extract_links(doc)
    undocumented_args = []
    undocumented_arg_types = []
    for (i, arg) in enumerate(function.function.args):
        if isinstance(function.function, FunctionDefinition):
            (type, name) = arg
        else:
            type = ''
            name = arg

        # Methods don't need to document the object arg.
        if i == 0 and name == 'object':
            continue

        # Skip variable args for now
        if type == '...':
            continue

        # Skip callback objects for now
        if name == 'callback_object':
            continue

        if name not in links:
            undocumented_args.append(name)

        if isinstance(type, Type) and type.name == 'UtObject*' and \
           get_arg_type(function.tags, name) is None:
            undocumented_arg_types.append(name)

    if len(undocumented_args) > 0:
        undocumented_function_args.append((function, undocumented_args))
    if len(undocumented_arg_types) > 0:
        undocumented_function_arg_types.append(
            (function, undocumented_arg_types))

    type_name = None
    if isinstance(function.function, FunctionDefinition) and \
       isinstance(function.function.return_type, Type):
        type_name = function.function.return_type.name
    if type_name == 'UtObject*':
        if get_return_type(function.tags) is None:
            undocumented_function_return_types.append(function)
    elif type_name == 'char*':
        if get_return_type(function.tags) is not None:
            invalid_function_return_types.append(function)
    else:
        if get_return_type(function.tags) is not None or \
           has_return_ref(function.tags):
            invalid_function_return_types.append(function)

    invalid_arg_types = []
    for tag in function.tags:
        if tag[0] == 'arg-type' and not function.function.has_arg(tag[1]):
            invalid_arg_types.append(tag[1])
    if len(invalid_arg_types) > 0:
        invalid_function_arg_types.append((function, invalid_arg_types))

enums.sort(key=lambda x: x.name)
undocumented_enums = []
undocumented_enum_values = []
for enum in enums:
    doc_text += '\n'
    doc_text += enum.name + '\n'
    doc = make_doc(enum.comments)
    doc_text += doc + '\n'
    if len(enum.comments) == 0:
        undocumented_enums.append(enum)
        continue
    links = extract_links(doc)
    undocumented_values = []
    for (name, value) in enum.enum.values:
        if name not in links:
            undocumented_values.append(name)
    if len(undocumented_values) > 0:
        undocumented_enum_values.append((enum, undocumented_values))

f = open('API.md', 'w')
f.write(doc_text)

fully_documented = True

if len(undocumented_functions) > 0:
    fully_documented = False
    print('Undocumented functions:')
    for function in undocumented_functions:
        print('  ' + function.function.name)
if len(undocumented_function_args) > 0:
    fully_documented = False
    print('Undocumented function arguments:')
    for (function, args) in undocumented_function_args:
        print('  ' + function.function.name + ': ' + ', '.join(args))
if len(undocumented_function_arg_types) > 0:
    fully_documented = False
    print('Undocumented function argument types:')
    for (function, args) in undocumented_function_arg_types:
        print('  ' + function.function.name + ': ' + ', '.join(args))
if len(invalid_function_arg_types) > 0:
    fully_documented = False
    print('Invalid function argument types:')
    for (function, args) in invalid_function_arg_types:
        print('  ' + function.function.name + ': ' + ', '.join(args))
if len(undocumented_function_return_types) > 0:
    fully_documented = False
    print('Undocumented function return types:')
    for function in undocumented_function_return_types:
        print('  ' + function.function.name)
if len(invalid_function_return_types) > 0:
    fully_documented = False
    print('Invalid function return types:')
    for function in invalid_function_return_types:
        print('  ' + function.function.name)
n_elements = len(functions)
n_documented = n_elements - len(undocumented_functions)
print('Documented %d/%d functions' % (n_documented, n_elements))

if len(undocumented_enums) > 0:
    fully_documented = False
    print('Undocumented enums:')
    for enum in undocumented_enums:
        print('  ' + enum.name)
n_elements = len(enums)
n_documented = n_elements - len(undocumented_enums)
print('Documented %d/%d enums' % (n_documented, n_elements))
if len(undocumented_enum_values) > 0:
    fully_documented = False
    print('Undocumented enum values:')
    for (enum, values) in undocumented_enum_values:
        print('  ' + enum.name + ': ' + ', '.join(values))

if not fully_documented:
    sys.exit(1)
