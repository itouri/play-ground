using System;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

static public class Interact
{
    private static NameValueCollection ParseQueryString(string query)
    {
        var ret = new NameValueCollection();
        query = query.Trim('?');
        foreach (string pair in query.Split('&'))
        {
            string[] kv = pair.Split('=');

            string key = kv.Length == 1
              ? null : Uri.UnescapeDataString(kv[0]).Replace('+', ' ');

            string[] values = Uri.UnescapeDataString(
              kv.Length == 1 ? kv[0] : kv[1]).Replace('+', ' ').Split(',');

            foreach (string value in values)
            {
                ret.Add(key, value);
            }
        }
        return ret;
    }

    static public string GetOneValueWithKey(string key)
    {
        var query = ParseQueryString("?room_id=1234");
        if (query[key] == null)
        {
            return "";
        }
        Console.WriteLine(query[key].Length);
        return query[key];
    }

    static void Main() {
        var room_id = GetOneValueWithKey("room_id");
        Console.WriteLine(room_id);
    }
}