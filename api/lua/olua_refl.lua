
-- Intrinsic
olua_refl =
{
    __intrinsic_type_info       = {},
    __array_type_info           = {},
    __map_type_info             = {},
    __enum_type_info            = {},
    __function_type_info        = {},
    __class_type_info           = {},

    __intrinsic_type_info_int   = {},
    __intrinsic_type_info_float   = {},
    __intrinsic_type_info_string   = {},
    __intrinsic_type_info_bool   = {},
    __intrinsic_type_info_any   = {},

    fetch_type = function(def)
        local deftype = type(def)
        if(deftype == "function" ) then
            return def()
        elseif(deftype == "table") then
            return def;
        else
            return nil;
        end

    end
}


function olua_refl.__intrinsic_type_info:type()
    return 1
end

function olua_refl.__intrinsic_type_info:value_type()
    return self.__value_type
end

-- Array

function olua_refl.__array_type_info:type()
    return 2
end
function olua_refl.__array_type_info:element_type()
    return olua_refl.fetch_type(self.__element_type);
end

function olua_refl.__array_type_info:element_nilable()
    return self.__element_nilable;
end

-- Map

function olua_refl.__map_type_info:type()
    return 3
end

function olua_refl.__map_type_info:key_type()
    return olua_refl.fetch_type(self.__key_type)
end


function olua_refl.__map_type_info:value_type()
    return olua_refl.fetch_type(self.__value_type)
end
function olua_refl.__map_type_info:value_nilable()
    return self.__value_nilable
end

-- Enum

function olua_refl.__enum_type_info:type()
    return 4
end

function olua_refl.__enum_type_info:enum_name()
    return self.__enum_name;
end

function olua_refl.__enum_type_info:item_count()
    return #(self.__items)
end

function olua_refl.__enum_type_info:items()
    return self.__items
end

function olua_refl.__enum_type_info:parse(str)
    return olua_ext.enum_parse(self.__items, str)
end

function olua_refl.__enum_type_info:tostring(val)
    return olua_ext.enum_tostring(self.__items, val)
end


-- Function
function olua_refl.__function_type_info:type()
    return 5
end

function olua_refl.__function_type_info:self_type()
    return olua_refl.fetch_type(self.__self_type)
end

function olua_refl.__function_type_info:param_count()
    return #(self.__params)
end

function olua_refl.__function_type_info:param_type(idx)
    return olua_refl.fetch_type(self.__params[idx].__type)
end

function olua_refl.__function_type_info:param_nilable(idx)
    return self.__params[idx].__nilable
end

function olua_refl.__function_type_info:param_optional(idx)
    return self.__params[idx].__optional
end

function olua_refl.__function_type_info:param_const(idx)
    return self.__params[idx].__const
end

function olua_refl.__function_type_info:param_variable(idx)
    return self.__params[idx].__variable
end

function olua_refl.__function_type_info:return_count()
    return #(self.__returns)
end

function olua_refl.__function_type_info:return_type(idx)
    return olua_refl.fetch_type(self.__returns[idx].__type)
end

function olua_refl.__function_type_info:return_nilable(idx)
    return self.__return[idx].__nilable
end


-- Class

function olua_refl.__class_type_info:type()
    return 6
end

function olua_refl.__class_type_info:class_name()
    return self.__class_name
end

function olua_refl.__class_type_info:base_count()
    return #(self.__base_types)
end

function olua_refl.__class_type_info:base_type(idx)
    return olua_refl.fetch_type(self.__base_types[idx].__type)
end

function olua_refl.__class_type_info:member_count()
    return #(self.__members)
end

function olua_refl.__class_type_info:member_name(idx)
    return self.__members[idx].__name
end

function olua_refl.__class_type_info:member_type(idx)
    return olua_refl.fetch_type(self.__members[idx].__type)
end
function olua_refl.__class_type_info:member_access(idx)
    return self.__members[idx].__access
end

function olua_refl.__class_type_info:member_is_static(idx)
    return self.__members[idx].__is_static
end

function olua_refl.__class_type_info:member_is_variant(idx)
    return self.__members[idx].__is_variant
end

function olua_refl.__class_type_info:member_is_constructor(idx)
    return self.__members[idx].__is_constructor
end

function olua_refl.__class_type_info:member_nilable(idx)
    return self.__members[idx].__nilable
end

function olua_refl.__class_type_info:create_object(ctor_name, ...)
    for i = 1, #self.__members do
        if(self.__members[i].__is_constructor == true and self.__members[i].__name == ctor_name) then
            local fn_type = olua_refl.fetch_type(self.__members[i].__type)
            return fn_type.__fn(...)
        end
    end
    return nil
end

function olua_refl.__class_type_info:set_member_value(inst, name, val)
    for i = 1, #self.__members do
        if(self.__members[i].__is_variant == true and self.__members[i].__name == name) then
            if(self.__members[i].__is_static) then
                self.__get_static_table()[name] = val
            else
                inst[name] = val
            end
            return
        end
    end
end

function olua_refl.__class_type_info:get_member_value(inst, name, val)
    for i = 1, #self.__members do
        if(self.__members[i].__is_variant == true and self.__members[i].__name == name) then
            if(self.__members[i].__is_static) then
                return self.__get_static_table()[name];
            else
                return inst[name];
            end
        end
    end
    return nil
end


function __init_reflection()
    olua_refl.__intrinsic_type_info_int = setmetatable({ __value_type = 0 }, { __index = olua_refl.__intrinsic_type_info});
    olua_refl.__intrinsic_type_info_float = setmetatable({ __value_type = 1 }, { __index = olua_refl.__intrinsic_type_info});
    olua_refl.__intrinsic_type_info_string = setmetatable({ __value_type = 2 }, { __index = olua_refl.__intrinsic_type_info});
    olua_refl.__intrinsic_type_info_bool = setmetatable({ __value_type = 3 }, { __index = olua_refl.__intrinsic_type_info});
    olua_refl.__intrinsic_type_info_any = setmetatable({ __value_type = 4 }, { __index = olua_refl.__intrinsic_type_info});
end


