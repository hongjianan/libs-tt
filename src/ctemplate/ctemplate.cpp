/*
 * scan_file.cpp
 *
 *  
 *
 *  Created on: 2016��11��12��
 *      Author: Hong
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <ctemplate/template.h>

using namespace ctemplate;

/* ============== main ==============*/
int ctemplate_tools(int argc, char *argv[])
{
    ctemplate::TemplateDictionary dict("example");
    dict.SetValue("table1_name", "example");

    // 为节省篇幅，这里只循环一次
    for (int i = 0; i < 2; ++i)
    {
        ctemplate::TemplateDictionary* table1_dict;
        table1_dict = dict.AddSectionDictionary("TABLE1");
        table1_dict->SetValue("field1", "1");
        table1_dict->SetValue("field2", "2");

        // 这里有点类似于printf
        table1_dict->SetFormattedValue("field3", "%d", i);

        for (int j = 0; j < 2; ++j)
        {
            ctemplate::TemplateDictionary* table2_dict;
            table2_dict = table1_dict->AddSectionDictionary("TABLE2");
            table1_dict->SetFormattedValue("field4", "%d", j);
        }
    }

    std::string output;
    ctemplate::Template* tpl;
    tpl = ctemplate::Template::GetTemplate("example.htm", ctemplate::DO_NOT_STRIP);
    tpl->Expand(&output, &dict);
    printf("%s\n", output.c_str());

    return 0;
}

/* end of file */
