#include "souistd.h"
#include "core/SwndLayout.h"
#include "helper/SplitString.h"

namespace SOUI
{
    
    //////////////////////////////////////////////////////////////////////////
    //
    SwndLayout::SwndLayout()
    :nCount(0)
    ,uPositionType(SizeX_FitContent | SizeY_FitContent)
    ,fOffsetX(0.0f)
    ,fOffsetY(0.0f)
    ,uSpecifyWidth(0)
    ,uSpecifyHeight(0)
    {
    }
    
    void SwndLayout::Clear()
    {
        nCount=0;
        uPositionType= SizeX_FitContent | SizeY_FitContent;
        fOffsetX=0.0f;
        fOffsetY=0.0f;
    }

    BOOL SwndLayout::IsEmpty()
    {
        return uPositionType == 0 && nCount ==0;
    }

    BOOL SwndLayout::IsFitParent(POSDIR pd)  const
    {
        switch(pd)
        {
        case PD_X: return uPositionType & SizeX_FitParent;
        case PD_Y: return uPositionType & SizeY_FitParent;
        case PD_ALL:return (uPositionType & (SizeX_FitParent | SizeY_FitParent));
        default:SASSERT(FALSE);return FALSE;
        }
    }

    BOOL SwndLayout::IsFitContent(POSDIR pd)  const
    {
        switch(pd)
        {
        case PD_X: return uPositionType & SizeX_FitContent;
        case PD_Y: return uPositionType & SizeY_FitContent;
        case PD_ALL:return (uPositionType & (SizeX_FitContent | SizeY_FitContent));
        default:SASSERT(FALSE);return FALSE;
        }
    }

    BOOL SwndLayout::IsSpecifySize( POSDIR pd )  const
    {
        switch(pd)
        {
        case PD_X: return uPositionType & SizeX_Specify;
        case PD_Y: return uPositionType & SizeY_Specify;
        case PD_ALL:return (uPositionType & (SizeX_Specify | SizeY_Specify));
        default:SASSERT(FALSE);return FALSE;
        }
    }
    
    int SwndLayout::GetSpecifySize(POSDIR pd) const
    {
        if(nCount != 4)
        {
            switch(pd)
            {
            case PD_X: return uSpecifyWidth;
            case PD_Y: return uSpecifyHeight;
            default:SASSERT(FALSE);break;
            }
        }else
        {
            switch(pd)
            {
            case PD_X: 
                if(pos[2].pit == PIT_SIZE)
                {
                    SASSERT(pos[2].cMinus == 1);
                    return (int)pos[2].nPos;
                }
                break;
            case PD_Y: 
                if(pos[3].pit == PIT_SIZE)
                {
                    SASSERT(pos[3].cMinus == 1);
                    return (int)pos[3].nPos;
                }
                break;
            default:SASSERT(FALSE);break;
            }                
        }
        return 0;
    }

    BOOL SwndLayout::InitPosFromString( const SStringW & strPos )
    {
        SStringWList strLst;
        SplitString(strPos,L',',strLst);
        if(strLst.GetCount() != 2 && strLst.GetCount() != 4) 
        {
            SASSERT_FMTW(L"Parse pos attribute failed, strPos=%s",strPos);
            return FALSE;
        }
        //增加pos属性中的空格兼容。
        for(size_t i=0;i<strLst.GetCount();i++)
        {
            strLst.GetAt(i).TrimBlank();
        }
        BOOL bRet = TRUE;

        bRet = ParsePosition12(strLst[0],strLst[1]);
        if(strLst.GetCount() == 4)
        {
            bRet = ParsePosition34(strLst[2],strLst[3]);
        }
        if(bRet && nCount == 4)
        {//检测X,Y方向上是否为充满父窗口
            uPositionType &= ~(SizeX_Mask|SizeY_Mask);
            
            if((pos[0].pit == PIT_NORMAL && pos[0].nPos == 0 && pos[0].cMinus==1)
                &&(pos[2].pit == PIT_NORMAL && pos[2].nPos == 0 && pos[2].cMinus==-1))
                SetFitParent(PD_X);
            else if(pos[2].pit == PIT_SIZE)
            {   
                if(pos[2].cMinus == -1)
                    SetFitContent(PD_X);
                else
                    SetWidth((int)pos[2].nPos);
            }
                
            if((pos[1].pit == PIT_NORMAL && pos[1].nPos == 0 && pos[1].cMinus==1)
                &&(pos[3].pit == PIT_NORMAL && pos[3].nPos == 0 && pos[3].cMinus==-1))
                SetFitParent(PD_Y);
            else if(pos[3].pit == PIT_SIZE)
            {
                if(pos[3].cMinus == -1)
                    SetFitContent(PD_Y);
                else
                    SetHeight((int)pos[3].nPos);
            }
        }
        return bRet;
    }

    BOOL SwndLayout::InitOffsetFromString( const SStringW & strPos )
    {
        float fx,fy;
        int nSegs=swscanf(strPos,L"%f,%f",&fx,&fy);
        if(nSegs != 2) 
        {
            SASSERT_FMTW(L"Parse offset attribute failed, strPos=%s",strPos);
            return FALSE;
        }
        fOffsetX = fx;
        fOffsetY = fy;
        return TRUE;
    }

    BOOL SwndLayout::InitOffsetFromPos2Type( const SStringW & strPos2Type )
    {
        if (strPos2Type.IsEmpty()) return FALSE;
        SStringW strValue2=strPos2Type;
        strValue2.MakeLower();
        BOOL bRet=TRUE;
        if(strValue2 == L"lefttop")
            fOffsetX=fOffsetY=0.0f;
        else if(strValue2 == L"leftmid")
            fOffsetX=0.0f,fOffsetY=-0.5f;
        else if(strValue2 == L"leftbottom")
            fOffsetX=0.0f,fOffsetY=-1.0f;
        else if(strValue2 == L"midtop")
            fOffsetX=-0.5f,fOffsetY=0.0f;
        else if(strValue2 == L"center")
            fOffsetX=-0.5f,fOffsetY=-0.5f;
        else if(strValue2 == L"midbottom")
            fOffsetX=-0.5f,fOffsetY=-1.0f;
        else if(strValue2 == L"righttop")
            fOffsetX=-1.0f,fOffsetY=0.0f;
        else if(strValue2 == L"rightmid")
            fOffsetX=-1.0f,fOffsetY=-0.5f;
        else if(strValue2 == L"rightbottom")
            fOffsetX=-1.0f,fOffsetY=-1.0f;
        else
            bRet=FALSE;
        return bRet;
    }

    BOOL SwndLayout::SetFitParent(POSDIR pd)
    {
        if(pd == 0) return FALSE;

        if(pd & PD_X)
        {
            uPositionType &= ~SizeX_Mask;
            uPositionType |= SizeX_FitParent;
        }
        if(pd & PD_Y)
        {
            uPositionType &= ~SizeY_Mask;
            uPositionType |= SizeY_FitParent;
        }

        return TRUE;
    }

    BOOL SwndLayout::SetFitContent(POSDIR pd)
    {
        if(pd == 0 ) return FALSE;

        if(pd & PD_X)
        {
            uPositionType &= ~SizeX_Mask;
            uPositionType |= SizeX_FitContent;
        }
        if(pd & PD_Y)
        {
            uPositionType &= ~SizeY_Mask;
            uPositionType |= SizeY_FitContent;
        }
        return TRUE;
    }

    BOOL SwndLayout::ParsePosition12( const SStringW & strPos1, const SStringW &strPos2 )
    {
        if(strPos1.IsEmpty() || strPos2.IsEmpty()) 
            return FALSE;
        POSITION_ITEM pos1,pos2;
        if(!StrPos2ItemPos(strPos1,pos1) || !StrPos2ItemPos(strPos2,pos2) )
            return FALSE;
        if(pos1.pit == PIT_SIZE || pos2.pit == PIT_SIZE)//前面2个属性不能是size类型
            return FALSE;
        pos [PI_LEFT] = pos1;
        pos [PI_TOP] = pos2;
        nCount = 2;
        return TRUE;
    }

    BOOL SwndLayout::ParsePosition34( const SStringW & strPos3, const SStringW &strPos4 )
    {
        if(strPos3.IsEmpty() || strPos4.IsEmpty()) return FALSE;
        POSITION_ITEM pos3,pos4;
        if(!StrPos2ItemPos(strPos3,pos3) || !StrPos2ItemPos(strPos4,pos4) ) return FALSE;

        pos [PI_RIGHT] = pos3;
        pos [PI_BOTTOM] = pos4;
        nCount = 4;
        return TRUE;
    }

    BOOL SwndLayout::StrPos2ItemPos( const SStringW &strPos,POSITION_ITEM & pos )
    {
        if(strPos.IsEmpty()) return FALSE;
        
        if(strPos.Left(4)==L"sib.")
        {
            int nOffset = 0;
            if(strPos.Mid(4,5) == L"left@")
            {
                pos.pit = PIT_SIB_LEFT;
                nOffset = 5;
                
            }else if(strPos.Mid(4,6) == L"right@")
            {
                pos.pit = PIT_SIB_RIGHT;
                nOffset = 6;
            }else if(strPos.Mid(4,4) == L"top@")
            {
                pos.pit = PIT_SIB_TOP;
                nOffset = 4;
            }else if(strPos.Mid(4,7) == L"bottom@")
            {
                pos.pit = PIT_SIB_BOTTOM;
                nOffset = 7;
            }else
            {
                return FALSE;
            }
            int nSibID = 0;
            int nValue = 0;
            SStringW strValue = strPos.Mid(4+nOffset);
            if(2 != swscanf(strValue,L"%d:%d",&nSibID,&nValue))
                return FALSE;
            if(nSibID == 0) 
                return FALSE;
                
            pos.nRefID = nSibID;
            if(nValue < 0)
            {
                pos.nPos = (float)(-nValue);
                pos.cMinus = -1;
            }else
            {
                pos.nPos = (float)nValue;
                pos.cMinus = 1;
            }
        }else
        {
            LPCWSTR pszPos = strPos;
            switch(pszPos[0])
            {
            case POSFLAG_REFCENTER: pos.pit=PIT_CENTER,pszPos++;break;
            case POSFLAG_PERCENT: pos.pit=PIT_PERCENT,pszPos++;break;
            case POSFLAG_REFPREV_NEAR: pos.pit=PIT_PREV_NEAR,pszPos++;break;
            case POSFLAG_REFNEXT_NEAR: pos.pit=PIT_NEXT_NEAR,pszPos++;break;
            case POSFLAG_REFPREV_FAR: pos.pit=PIT_PREV_FAR,pszPos++;break;
            case POSFLAG_REFNEXT_FAR: pos.pit=PIT_NEXT_FAR,pszPos++;break;
            case POSFLAG_SIZE:pos.pit=PIT_SIZE,pszPos++;break;
            default: pos.pit=PIT_NORMAL;break;
            }
            
            pos.nRefID = -1;//not ref sibling using id
            if(pszPos [0] == L'-')
            {
                pos.cMinus = -1;
                pszPos ++;
            }else
            {
                pos.cMinus = 1;
            }
            pos.nPos=(float)_wtof(pszPos);
        }
        
        return TRUE;
    }

    BOOL SwndLayout::InitSizeFromString( const SStringW & strSize )
    {
        SStringWList strLst;
        SplitString(strSize,L',',strLst);
        if(strLst.GetCount() != 2) return FALSE;
        
        return InitWidth(strLst[0]) && InitHeight(strLst[1]);
    }

    BOOL SwndLayout::InitWidth( const SStringW & strWidth )
    {
        if(strWidth.IsEmpty()) return FALSE;

        SStringW strValue = strWidth;
        strValue.MakeLower();
        
        BOOL bRet=FALSE;
        if(strValue == L"full")
        {
            bRet=SetFitParent(PD_X);
        }else
        {
            int nWid = _wtoi(strValue);
            if(nWid<0)//size<0，代表自动计算size
                bRet=SetFitContent(PD_X);
            else
                bRet=SetWidth(nWid);
        }
        return bRet;
    }

    BOOL SwndLayout::InitHeight( const SStringW & strHeight )
    {
        if(strHeight.IsEmpty()) return FALSE;

        SStringW strValue = strHeight;
        strValue.MakeLower();
        
        BOOL bRet =FALSE;
        if(strValue == L"full")
        {
            bRet=SetFitParent(PD_Y);
        }else
        {
            int nHei = _wtoi(strValue);
            if(nHei<0)//size<0，代表自动计算size
                bRet=SetFitContent(PD_Y);
            else
                bRet=SetHeight(nHei);
        }
        return bRet;
    }


    BOOL SwndLayout::SetWidth( UINT nWid )
    {
        uPositionType &= ~ SizeX_Mask;
        uPositionType |= SizeX_Specify;

        uSpecifyWidth = nWid;

        return TRUE;
    }

    BOOL SwndLayout::SetHeight( UINT nHei )
    {
        uPositionType &= ~ SizeY_Mask;
        uPositionType |= SizeY_Specify;

        uSpecifyHeight = nHei;

        return TRUE;
    }

}
