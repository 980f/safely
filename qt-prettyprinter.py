import dumper


def qdump__NumericalValue(d, value):
    iss = value['is'].integer()
    val = value['storage']
    if iss == 0:
        img = val.extractInteger(8, True)
    elif iss == 1:
        img = val.extractInteger(32, False)
    elif iss == 2:
        img = val.extractInteger(32, True)
    else:
        img = val.extractSomething('d', 64)
    d.putValue('%s[%d] %s' % (["Truthy", "Whole", "Counting", "Floating"][iss], iss, img))


def qdump__Text(d, value):
    d.putItem(value['ptr'])
    d.putBetterType("char *")


def StorableTypeEnum(code):
    return ["NotDefined", "Uncertain", "Textual", "Numerical", "Wad"][code]


def StorableQualityEnum(code):
    return ["Empty", "Defaulted", "Parsed", "Edited"][code]


def qdump__Storable(d, value):
    iss = value['type'].integer()
    if iss == 4:
        onion = value['wad']
    elif iss == 3:
        onion = value['number']
    else:
        onion = value['text']
        # if not d.isExpanded():
        #    with SubItem(d):
        #        d.putName("name")
        d.putItem(value['name'])
#    with SubItem(d):
#        d.putName("value")
#        d.putItem(onion)
# d.putItem("%s" % value['name'])
