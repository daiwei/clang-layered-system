#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import shutil
import argparse
import zipfile
import md5
import time

PKG_SYS =       '0000001'
PKG_APP =       '0000002'

def sumfile(fobj):
    m = md5.new()
    while True:
        d = fobj.read(8096)
        if not d:
            break
        m.update(d)
    return m.hexdigest()
#end of sumfile

def md5sum(fname):
    if fname == '-':
        ret = sumfile(sys.stdin)
    else:
        try:
            f = file(fname, 'rb')
        except:
            return 'Failed to open file'
        ret = sumfile(f)
        f.close()
    return ret
#end of md5sum

def pkg_sign(fname, ftype):
    print '===================='
    print '= Add Package Sign ='
    print '===================='
    print 'file name:', os.path.split(fname)[1]
    print 'type:     ', ftype

    md5 = md5sum(fname)
    print 'md5sum:   ', md5

    tm = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())
    print 'time:     ', tm

    fp = open(fname, 'a')
    fp.write(md5 + ';' + tm + ';' + ftype)
    fp.flush()
    fp.close();
#end of md5sum

def zip_write_dir(zipFile, dirpath, dirname):
    curpath = os.getcwd()
    os.chdir(dirpath)

    for dirpath, dirnames, filenames in os.walk(dirname):
        if dirpath.startswith('./'):
            dirpath = dirpath[2:]

        # print 'dirpath:' + dirpath
        for filename in filenames:
            # print 'filename:' + filename
            if filename.startswith('.'):
                continue

            zipFile.write(os.path.join(dirpath, filename))

        if len(filenames) == 0:
            zif = zipfile.ZipInfo(dirpath + '/')
            zipFile.writestr(zif, '')

    os.chdir(curpath)
#end of zip_write_dir

class AddAppAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        appname = values[0]
        os.chdir('app')

        os.mkdir(appname)
        os.chdir(appname)
        shutil.copy('../../tool/app.icon', '.')

        fcm = open('CMakeLists.txt', mode='w')
        # fcm.write(str)
        # fcm.writelines(seq)
        # fcm.flush()

        os.mkdir('src')
        os.mkdir('lib')
        os.mkdir('resource')
        os.mkdir('user-data')

        os.chdir('..')
#end of AddAppAction

class PkgAppAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        appname = values[0]
        os.chdir('app/' + appname)

        zipFile = zipfile.ZipFile('../' + appname + '.app', 'w', zipfile.ZIP_DEFLATED)
        zipFile.write('app.icon')
        zipFile.write('app.so')
        zip_write_dir(zipFile, '.', 'lib');
        zip_write_dir(zipFile, '.', 'resource');
        zip_write_dir(zipFile, '.', 'user-data');

        zipFile.close()

        pkg_sign('../' + appname + '.app', PKG_APP)
        os.chdir('../..')
#end of PkgAppAction

class PkgSysAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        os.chdir('build')

        zipFile = zipfile.ZipFile('../sys.img', 'w', zipfile.ZIP_DEFLATED)
        zip_write_dir(zipFile, '.', '.');

        zipFile.close()

        pkg_sign('../sys.img', PKG_SYS)
        os.chdir('..')
#end of PkgSysAction

def main():
    # change work dir
    realpath = os.path.split( os.path.realpath( sys.argv[0] ) )[0]
    os.chdir(realpath + '/..')

    # parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--new-app', nargs=1, action=AddAppAction)
    parser.add_argument('--pkg-app', nargs=1, action=PkgAppAction)
    parser.add_argument('--pkg-sys', nargs=0, action=PkgSysAction)
    args = parser.parse_args()

    return 0
#end of main


if __name__ == '__main__':
    main()
