#!/usr/bin/env python
import cgi, os, time,sys
form = cgi.FieldStorage()
val = form.getvalue('val')

s2fName = '/tmp/s2f_rg'
f2sName = '/tmp/f2s_rg'
s2f = open(s2fName,'w+')
f2s = open(f2sName,'r',0)
print  >> sys.stderr ,"bonjour\n"
s2f.write("w %s\n" % val)
print  >> sys.stderr ,"A\n"
s2f.flush()
print  >> sys.stderr ,"B\n"
res = f2s.readline()
print  >> sys.stderr ,"C\n"
f2s.close()
s2f.close()
print  >> sys.stderr ,"D\n"

html="""
<head>
  <title>Peri Web Server</title>
  <META HTTP-EQUIV="Refresh" CONTENT="1; URL=/cgi-bin/main.py">
</head>
<body>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val" cols="40"></input>
  <input type="submit" value="Entrer">
  set %s
</form>
</body>
""" % (val,)

print html
