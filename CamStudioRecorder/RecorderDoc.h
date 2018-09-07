#pragma once

class CRecorderDoc : public CDocument
{
protected:
    CRecorderDoc();
    DECLARE_DYNCREATE(CRecorderDoc)
public:
    BOOL OnNewDocument() override;
    void Serialize(CArchive &ar) override;
public:
    ~CRecorderDoc() override;
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext &dc) const override;
#endif

protected:
    DECLARE_MESSAGE_MAP()
};
