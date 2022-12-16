olua_ext = 
{
    string_to_bool = function( str )
        if(str == "1" or str == "true" or str == "TRUE" or str == "True") then
            return true;
        end
        return false;
    end
    ,

    table_clear  = function ( tab )
        for k, v in pairs(tab) do
            rawset(tab, k, nil)
        end
    end
    ,
    

    enum_tostring = function (enum_static, val)
        for k, v in pairs(enum_static) do
            if(v == val) then
                return k
            end
        end
        return "(none)"
    end
    ,
    enum_parse = function (enum_static, name)
        for k, v in pairs(enum_static) do
            if(k == name) then
                return v
            end
        end
        return nil
    end


}

