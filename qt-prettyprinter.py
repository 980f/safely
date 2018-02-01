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
        safely__StorableWad(d,value)
        return
    elif iss == 3:
        onion = value['number']
    else:  #others are all varying degrees of confidence that the image is actually text.
        onion = value['text']

    d.putItem(onion)
    d.putBetterType(onion.type)

def safely__StorableWad(d,storable):
    innerType = 'Storable *'  #our wad is always mroe of us
    value = storable['wad']['v']  #named for what I copied out of stdtypes.py
    # the next chunk is pulled from std__vector
    (start, finish, alloc) = value.split("ppp")
    size = int((finish - start) / innerType.size())
    d.check(finish <= alloc)
    if size > 0:
        d.checkPointer(start)
        d.checkPointer(finish)
        d.checkPointer(alloc)
    d.check(0 <= size and size <= 1000 * 1000 * 1000)
    d.putItemCount(size)
    #back to piece taken from std_map
    if d.isExpanded():
        keyType = 'Text'  #our text class is used for name members
        valueType = 'Storable *'  #and this is what the vector is filled with, value.type[0] might be the same thing.
        with Children(d, size, maxNumChild=1000):
            # node = value["_M_t"]["_M_impl"]["_M_header"]["_M_left"]
            # nodeSize = node.dereference().type.size()
            # typeCode = "@{%s}@{%s}" % (keyType.name, valueType.name)
            for i in d.childRange():
                q = start + i * 4   #! sizeof pointer
                with SubItem(d, i):
                    kidstar = d.extractPointer(q)
                    kid=d.dereference(kidstar)
                    key=kid['name']
                    d.putPairItem(i, (key, kid))
