#pragma once

const WCHAR* szFilters = _T("媒体文件(所有媒体文件)\0*.flv;*.f4v;*.flc;*.3gpp;*.3g2;*.mkv;\
*.ram;*.fli;*.3gp;*.rmvb;*.mp4;*.mov;*.rm;*.asf;*.wmv;*.m1v;*.ts;*.wma;*.vob;*.m2ts;*.mpg;\
*.mpeg;*.mpe;*.m2v;*.m2p;*.m4v;*.wav;*.arm;*.m4b;*.ogv;*.aac;*.flac;*.mp3;*.avi;*.ac3;*.acc;*.aif;*.aiff;*.aifc;\
*.amr;*.ape;*.au;*.dat;*.dv;*.m4a;*.m4r;*.mka;*.mp2;*.mpc;*.ogg;*.ra;*.swf;*.wv;*.webm;\
*.mpeg4;*.caf;*.opus;*.spx;*.gsm;*.qt;*.dts;*.evo;*.dvr-ms;*.mod;*.amv;*.bik;*.tp;\
*.mpv2;*.mp2v;*.pva;*.3gp2;*.divx;*.h264;*.k3g;*.mts;*.nsv;*.ogm;*.pmf;*.pmp;\
*.pss;*.scm;*.tod;*.vp6;*.vp7;*.wtv;*.wmp;*.cda;*.tta;*.mid;*.rmi\0")
_T("视频文件(所有视频文件)\0*.flv;*.ogv;*.f4v;*.m1v;*.flc;*.3gpp;*.mkv;*.avi;*.ram;*.fli;*.3gp;*.rmvb;\
*.mp4;*.mov;*.rm;*.asf;*.wmv;*.ts;*.vob;*.m2ts;*.mpg;*.mpeg;*.m2v;*.m2p;*.m4v;*.3g2;\
*.dv;*.mpe;*.mpeg4;*.mp2v;*.swf;*.webm;*.dat;*.gsm;*.qt;*.evo;*.dvr-ms;*.mod;\
*.mpv2;*.amv;*.bik;*.tp;*.pva;*.3gp2;*.divx;*.h264;*.k3g;*.mts;*.nsv;*.ogm;*.pmf;*.pmp;\
*.pss;*.scm;*.tod;*.vp6;*.vp7;*.wtv\0")
_T("音频文件(所有音频文件)\0*.wav;*.m4b;*.wma;*.mpc;*.flac;*.arm;*.aac;*.lclac;*.mp3;*.mp2;*.ac3;*.acc;\
*.aif;*.aiff;*.aifc;*.amr;*.ape;*.au;*.m4a;*.m4r;*.mka;*.ogg;*.ra;*.wv;*.caf;*.opus;\
*.wmp;*.spx;*.dts;*.cda;*.tta;*.mid;*.rmi\0")
    _T("所有文件(*.*)\0*.*\0");

const wstring szView[] = {
    L".flv",L".f4v",L".flc",L".3gpp",L".ogv",L".m1v",L".mkv",L".avi",L".ram",L".fli" ,L".3gp",L".rmvb",L".mp4",L".mov",
    L".rm",L".asf",L".wmv",L".ts",L".vob",L".m2ts" ,L".mpg",L".mpeg",L".m2v",L".m2v",L".m2p",
    L".m4v",L".3g2",L".dv",L".swf",L".webm",L".dat",L".gsm",L".qt",L".evo",L".dvr-ms",
    L".mod",L".amv",L".bik",L".tp",L".mp2v",L".mpe",L".mpeg4",L".mpv2",L".pva",L".3gp2",
    L".divx",L".h264",L".k3g",L".mts",L".nsv",L".ogm",L".pmf",L".pmp",L".pss",
    L".scm",L".tod",L".vp6",L".vp7",L".wtv"
};

const wstring szAudio[] = {
    L".wav",L".flac",L".arm",L".m4b",L".wma",L".aac",L".lclac",L".mp3",L".mp2",L".ac3",L".acc",L".aif",L".aiff",L".mpc",
    L".aifc",L".amr",L".dts",L".ape",L".au",L".m4a",L".m4r",L".mka",L".ogg",L".ra",
    L".wv",L".caf",L".opus",L".spx",L".wmp",L".cda",L".tta",L".mid",L".rmi"
};

const wstring szSubFormat[] = {
    L".sub",L".srt",L".ssa",L".ass"
};

