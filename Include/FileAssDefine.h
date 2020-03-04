
enum _ExtID
{
	EXT_ID_asf = 4000,
	EXT_ID_avi,
	EXT_ID_wmv,
	EXT_ID_wmp, //w
	EXT_ID_dvrms, //w

	EXT_ID_rmvb,
	EXT_ID_ram,
	EXT_ID_rm,
	EXT_ID_rpm,//
	EXT_ID_scm,//
	EXT_ID_rp,//
	EXT_ID_rt,//
	EXT_ID_smil,//

	EXT_ID_mov,
	EXT_ID_qt,
		
	EXT_ID_m1v,//
	EXT_ID_m2v,
	EXT_ID_m2p,
	EXT_ID_m2ts,
	EXT_ID_mp2v,//
	EXT_ID_mpe,
	EXT_ID_mpeg,
	EXT_ID_mpeg1,//
	EXT_ID_mpeg2,//
	EXT_ID_mpg,
	EXT_ID_mpv2,//
	EXT_ID_pss,//
	EXT_ID_pva,//
	EXT_ID_tp,//
	EXT_ID_trp,//
	EXT_ID_ts,

	EXT_ID_mp4,
	EXT_ID_m4b,//
	EXT_ID_m4r,
	EXT_ID_m4v,
	EXT_ID_mpeg4,//

	EXT_ID_3g2,//
	EXT_ID_3gp,
	EXT_ID_3gp2,//
	EXT_ID_3gpp,

	EXT_ID_flv,
	EXT_ID_f4v,
	EXT_ID_swf,

	EXT_ID_vob,
	EXT_ID_ifo,//

	EXT_ID_amv,//
	EXT_ID_csf,//
	EXT_ID_divx,//
	EXT_ID_evo,//
	EXT_ID_mkv,
	EXT_ID_mod,//
	EXT_ID_pmp,//
	EXT_ID_vp6,//
	EXT_ID_bik,//
	EXT_ID_mts,//
	EXT_ID_xvx,//
	EXT_ID_xv,//
	EXT_ID_xlmv,//
	EXT_ID_ogm,//
	EXT_ID_ogv,//
	EXT_ID_ogx,//
	EXT_ID_m2t,
	EXT_ID_h264,//
	EXT_ID_k3g,//
	EXT_ID_nsv,//
	EXT_ID_pmf,//
	EXT_ID_tod,//
	EXT_ID_vp7,//
	EXT_ID_webm,
	EXT_ID_wtv,//

	EXT_ID_aac,
	EXT_ID_ac3,
	EXT_ID_acc,
	EXT_ID_aiff,
	EXT_ID_amr,
	EXT_ID_ape,
	EXT_ID_au,
	EXT_ID_cda,//
	EXT_ID_dts,//
	EXT_ID_flac,
	EXT_ID_m4a,
	EXT_ID_mka,
	EXT_ID_mp2,
	EXT_ID_mp3,
	EXT_ID_mpc,
	EXT_ID_ra,
	EXT_ID_tta,//
	EXT_ID_wav,
	EXT_ID_wma,
	EXT_ID_wv,
	EXT_ID_mid,//
	EXT_ID_ogg,
	EXT_ID_oga,//
	EXT_ID_rmi,//
	EXT_ID_aif,

	EXT_ID_fli,
	EXT_ID_flc,
	EXT_ID_arm,
	EXT_ID_aifc,
	EXT_ID_caf,
	EXT_ID_opus,
	EXT_ID_spx,
	EXT_ID_gsm,
};

struct _Ext
{
	int nExt;
	const char* ext;
};

const _Ext AllExt[] = {
	{ EXT_ID_asf,".asf" },{ EXT_ID_avi,".avi" },{ EXT_ID_wmv,".wmv" },{ EXT_ID_wmp,".wmp" },{ EXT_ID_dvrms,".dvr-ms" },
	{ EXT_ID_rmvb,".rmvb" },{ EXT_ID_ram,".ram" },{ EXT_ID_rm,".rm" },{ EXT_ID_rpm,".rpm" },{ EXT_ID_scm,".scm" },
	{ EXT_ID_rp,".rp" },{ EXT_ID_rt,".rt" },{ EXT_ID_smil,".smil" },{ EXT_ID_mov,".mov" },{ EXT_ID_qt,".qt" },
	{ EXT_ID_m1v,".m1v" },{ EXT_ID_m2v,".m2v" },{ EXT_ID_m2p,".m2p" },{ EXT_ID_m2ts,".m2ts" },
	{ EXT_ID_mp2v,".mp2v" },{ EXT_ID_mpe,".mpe" },{ EXT_ID_mpeg,".mpeg" },{ EXT_ID_mpeg1,".mpeg1" },{ EXT_ID_mpeg2,".mpeg2" },
	{ EXT_ID_mpg,".mpg" },{ EXT_ID_mpv2,".mpv2" },{ EXT_ID_pss,".pss" },{ EXT_ID_pva,".pva" },{ EXT_ID_tp,".tp" },
	{ EXT_ID_ts,".ts" },{ EXT_ID_mp4,".mp4" },{ EXT_ID_m4b,".m4b" },{ EXT_ID_m4r,".m4r" },
	{ EXT_ID_m4v,".m4v" },{ EXT_ID_mpeg4,".mpeg4" },{ EXT_ID_3g2,".3g2" },{ EXT_ID_3gp,".3gp" },{ EXT_ID_3gp2,".3gp2" },
	{ EXT_ID_3gpp,".3gpp" },{ EXT_ID_flv,".flv" },{ EXT_ID_f4v,".f4v" },{ EXT_ID_swf,".swf" },{ EXT_ID_vob,".vob" },
	{ EXT_ID_amv,".amv" },{ EXT_ID_divx,".divx" },{ EXT_ID_evo,".evo" },
	{ EXT_ID_mkv,".mkv" },{ EXT_ID_mod,".mod" },{ EXT_ID_pmp,".pmp" },{ EXT_ID_vp6,".vp6" },{ EXT_ID_bik,".bik" },
	{ EXT_ID_mts,".mts" },{ EXT_ID_xvx,".xvx" },{ EXT_ID_xv,".xv" },{ EXT_ID_xlmv,".xlmv" },{ EXT_ID_ogm,".ogm" },
	{ EXT_ID_ogv,".ogv" },{ EXT_ID_ogx,".ogx" },{ EXT_ID_h264,".h264" },{ EXT_ID_k3g,".k3g" },
	{ EXT_ID_nsv,".nsv" },{ EXT_ID_pmf,".pmf" },{ EXT_ID_tod,".tod" },{ EXT_ID_vp7,".vp7" },{ EXT_ID_webm,".webm" },
	{ EXT_ID_wtv,".wtv" },{ EXT_ID_aac,".aac" },{ EXT_ID_ac3,".ac3" },{ EXT_ID_acc,".acc" },{ EXT_ID_aiff,".aiff" },
	{ EXT_ID_amr,".amr" },{ EXT_ID_ape,".ape" },{ EXT_ID_au,".au" },{ EXT_ID_cda,".cda" },{ EXT_ID_dts,".dts" },
	{ EXT_ID_flac,".flac" },{ EXT_ID_m4a,".m4a" },{ EXT_ID_mka,".mka" },{ EXT_ID_mp2,".mp2" },{ EXT_ID_mp3,".mp3" },
	{ EXT_ID_mpc,".mpc" },{ EXT_ID_ra,".ra" },{ EXT_ID_tta,".tta" },{ EXT_ID_wav,".wav" },{ EXT_ID_wma,".wma" },
	{ EXT_ID_wv,".wv" },{ EXT_ID_mid,".mid" },{ EXT_ID_ogg,".ogg" },{ EXT_ID_oga,".oga" },{ EXT_ID_rmi,".rmi" },
	{ EXT_ID_aif,".aif" },{ EXT_ID_fli,".fli" },{ EXT_ID_flc,".flc" },{ EXT_ID_arm,".arm" },{ EXT_ID_aifc,".aifc" },
	{ EXT_ID_caf,".caf" },{ EXT_ID_opus,".opus" },{ EXT_ID_spx,".spx" },{ EXT_ID_gsm,".gsm" },
	{ 0,nullptr }
};
