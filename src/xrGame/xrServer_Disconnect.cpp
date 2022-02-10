#include "stdafx.h"
#include "xrServer.h"
#include "file_transfer.h"
#include "screenshot_server.h"

void xrServer::Disconnect()
{
	IPureServer::Disconnect	();
	SLS_Clear				();
	xr_delete				(game);
}
