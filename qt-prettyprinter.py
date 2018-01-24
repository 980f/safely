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
    d.putValue('%s %s' % (["Truthy", "Whole", "Counting", "Floating"][iss], img))


def qdump__Cstr(d, value):
    d.putItem(value['ptr'])
    d.putBetterType("char *")


def qdump__Text(d, value):
    d.putItem(value['ptr'])
    d.putBetterType("char *")


def qdump__Index(d, value):
    raw = value['raw']
    if raw.integer() == 4294967295:
        d.putValue("BadIndex")
    else:
        d.putItem(raw)
        d.putBetterType("unsigned")


def qdump__Ordinal(d, value):
    d.putValue('%u/%u' % (value['pointer'], value['length']))


def StorableTypeEnum(code):
    return ["NotDefined", "Uncertain", "Textual", "Numerical", "Wad"][code]


def StorableQualityEnum(code):
    return ["Empty", "Defaulted", "Parsed", "Edited"][code]


def qdump__Storable(d, value):
    iss = value['type'].integer()
    if iss == 4:
        wad = value['wad']
        # size = wad['v']['size']
        onion = wad['v']
        # if d.isExpanded():
        #     d.numputNumChild(size)
        # else:
        #     onion = wad
    elif iss == 3:
        onion = value['number']
    else:  #others are all varying degrees of confidence that the image is actually text.
        onion = value['text']

    d.putItem(onion)
    d.putBetterType(onion.type)
