#pragma once

class CCommandEdit :
    public CWindowImpl<CCommandEdit, CEdit>,
    public CEditCommands<CCommandEdit>
{
    public:

    BEGIN_MSG_MAP(CCommandEdit)
       CHAIN_MSG_MAP_ALT(CEditCommands<CCommandEdit>, 1)
    END_MSG_MAP()
};

