#include "StdAfx.h"
#include "managerdll.h"
#include "ImportZamiennikD.h"

#include "../excel/excelzamiennikimport.h"
#include "../CommonZamiennikImport.h"
#include "../csv/CSVZamiennikiImport.h"
#include "dialogs/male/CSVParametryD.h"
#include "wti.h"
//Szczegolowe


// CImportZamiennikiD dialog

IMPLEMENT_DYNAMIC(CImportZamiennikiD, CMyDialog)
CImportZamiennikiD::CImportZamiennikiD()
: CMyDialog(CImportZamiennikiD::IDD, NULL)
{
    setRegSaveDataT(_T("Import\\Zamienniki"));

    m_i_cfiexcel.addRadio(&m_i_cfiexcel, _T("CFIEXCEL"));
    m_i_cfiexcel.addRadio(&m_i_cficsv, _T("CFICSV"));
    m_fileName.setFilter(getFileMask());

    setDefVal(getDataT(), _T("_format"), _T("CFIEXCEL"));

    csvconf = CCommonCSVConfig::getCSVConfig(eUstawieniaImportCSV_Zamienniki, true);
}

CImportZamiennikiD::~CImportZamiennikiD()
{
    saveRegSaveDataT();
}

void CImportZamiennikiD::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_FileEditCtrl(pDX, IDC_NAZWAPLIKU, m_fileName, FEC_FILEOPEN);
    DDX_Control(pDX, IDC_I_CFIEXCEL, m_i_cfiexcel);
    DDX_Control(pDX, IDC_I_CFICSV, m_i_cficsv);
    DDX_Control(pDX, IDC_CSV_PARAMS, m_csv_params);
    DDX_Control(pDX, IDC_C_OSTRANS, m_im_osTrans);
    DDX_Control(pDX, IDC_C_BLADPRZER, m_im_bladPrzerywa);
}

bool CImportZamiennikiD::RegisterOnInit()
{
    DC_REGISTERT("_fileName", m_fileName);
    DC_REGISTERT("_format", m_i_cfiexcel);
    DC_REGISTERT("_im_ostrans", m_im_osTrans);
    DC_REGISTERT("_im_bladprzerywa", m_im_bladPrzerywa);
    return __super::RegisterOnInit();
}

void CImportZamiennikiD::UkryjCoNiepotrzebne()
{
    _ENABLE_IT(m_csv_params);

    if (GET_POLE(_T("_format")) != _T("CFICSV"))
    {
        _DISABLE_IT(m_csv_params);
    }

    __super::UkryjCoNiepotrzebne();
    performEHOperations();
}


BEGIN_MESSAGE_MAP(CImportZamiennikiD, CMyDialog)
    ON_BN_CLICKED(IDC_CSV_PARAMS, OnBnCsv)
END_MESSAGE_MAP()

void CImportZamiennikiD::OnBnCsv()
{
    CCSVParametryD dlg(GET_POLE(_T("_fileName")), csvconf);
    dlg.DoModal();
}

class CImportZamiennikiD_DM : public CDialogManagerItem
{
public:
    bool executeForParam(const CMyString& param);

    DIALOGMANAGER_ITEMCREATE(CImportZamiennikiD_DM);
};


DIALOGMANAGER_REGISTERT("ImportZamiennikiD", CImportZamiennikiD_DM);


bool CImportZamiennikiD_DM::executeForParam(const CMyString& param)
{
    CImportZamiennikiD dlg;
    dlg.DoModal();
    return (dlg.getReturnValue() != NULLSTRING);
}


CMyString CImportZamiennikiD::getFileMask()
{
    return _FT("%sPliki CSV (*.csv)|*.csv|Wszystkie pliki (*.*)|*.*||",FILEDIALOG_PARAMS_XLS);
}


void CImportZamiennikiD::OnOK()
{
    _ileZaimportowano = 0;

    CCommonZamiennikImport* ci = NULL;
    if (m_i_cfiexcel.getValue() == _T("CFIEXCEL")) ci = new CExcelZamiennikImport();
    if (m_i_cfiexcel.getValue() == _T("CFICSV"))
    {
        ci = new CCSVZamiennikiImport();
        if (ci) ((CCSVZamiennikiImport*)ci)->setCSVConfig(csvconf);
    }

    ci->setHowActualize(getHowActualize());
    ci->setOsobneTransakcje(_atoi(DTGDTGETPOLET("_im_ostrans")));
    ci->setBladPrzerywa(_atoi(DTGDTGETPOLET("_im_bladprzerywa")));

    WTI_NDLG(*this, ci, 0);

    if (ci->isError())
    {
        AfxMultiMessageBox(ci->getErrorMessage());
        DELETE_WSK(ci);
        return;
    }

    DELETE_WSK(ci);

    AfxMessageBox(_TX("Zaimportowano ") + CMyString(_ileZaimportowano) + _T(" zamienników"));

    CMyDialog::OnOK();
}

void CImportZamiennikiD::executeItFromThreadTPL(CCommonStatusRet* sr, CCommonWTIParamTPL<CCommonZamiennikImport>& prm)
{
    CCommonZamiennikImport* eti = prm.getParam();
    if (eti)
    {
        eti->setStatusRetHandler(sr);
        CDataT::toLog(_FX(_TX("Rozpoczęto import %s zamienników z pliku  %s."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName"))), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        _ileZaimportowano = eti->execute(DTGDTGETPOLET("_fileName"));
        CDataT::toLog(_FX(_TX("Zakończono import %s z pliku %s, zaimportowano %s pozycji."), GET_POLE(_T("_format")), GET_POLE(_T("_fileName")), CMyString(_ileZaimportowano)), ZSYSTEMOWE_IMPORTDANYCH, TG_ZDARZENIASYSTEMOWE);
        eti->clearStatusRetHandler();
    }
}


int CImportZamiennikiD::getHowActualize()
{
    return ACTUALIZE_SKIP;
}
// CImportZamiennikiD message handlers

INT_PTR CImportZamiennikiD::DoModal()
{
    if (!CanDoIt(UPR_IMPORT_ZAMIENNIKI))
    {
        AfxMessageBox(_TX("Brak uprawnień!"));
        return 0;
    }
    return __super::DoModal();
}

CUInfo CImportZamiennikiD::getPermission(int no)
{
    switch (no)
    {
    case 0:
        return getMainPermission();
    }
    return __super::getPermission(no - 1);
}

CUInfo CImportZamiennikiD::getMainPermission()
{
    return addPrefix(getUprawnienie(UPR_IMPORT_ZAMIENNIKI));
}
