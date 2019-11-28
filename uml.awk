#!/usr/bin/gawk -f

# Execute for every lines
{
   umlkey = "//u ";
   pos = index($0, umlkey);
   if (pos) {
      print substr($0, pos + length(umlkey))
   }
}
