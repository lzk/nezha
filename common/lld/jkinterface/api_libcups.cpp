
#define _IPP_PRIVATE_STRUCTURES 1
#include "cups/cups-private.h"

#include "cups/cups.h"
#include "cups/http.h"
#include "cups/ipp.h"
#include <stdio.h>
#include <string.h>

#include "api_libcups.h"
#include "log.h"

/*
 * 'match_list()' - Match a name from a list of comma or space-separated names.
 */

static int				/* O - 1 on match, 0 on no match */
match_list(const char *list,		/* I - List of names */
           const char *name)		/* I - Name to find */
{
  const char	*nameptr;		/* Pointer into name */


 /*
  * An empty list always matches...
  */

  if (!list || !*list)
    return (1);

  if (!name)
    return (0);

  while (*list)
  {
   /*
    * Skip leading whitespace and commas...
    */

    while (isspace(*list & 255) || *list == ',')
      list ++;

    if (!*list)
      break;

   /*
    * Compare names...
    */

    for (nameptr = name;
     *nameptr && *list && tolower(*nameptr & 255) == tolower(*list & 255);
     nameptr ++, list ++);

    if (!*nameptr && (!*list || *list == ',' || isspace(*list & 255)))
      return (1);

    while (*list && !isspace(*list & 255) && *list != ',')
      list ++;
  }

  return (0);
}

static char *					/* O - Buffer */
jk_cupsStrDate(char   *buf,		/* I - Buffer */
             size_t bufsize,		/* I - Size of buffer */
         time_t timeval)		/* I - Time value */
{
  struct tm	*dateval;		/* Local date/time */
  char		temp[1024];		/* Temporary buffer */
  _cups_globals_t *cg = _cupsGlobals();	/* Per-thread globals */


  if (!cg->lang_default)
    cg->lang_default = cupsLangDefault();

  dateval = localtime(&timeval);

  if (cg->lang_default->encoding != CUPS_UTF8)
  {
    strftime(temp, sizeof(temp), "%c", dateval);
    cupsCharsetToUTF8((cups_utf8_t *)buf, temp, (int)bufsize, cg->lang_default->encoding);
  }
  else
    strftime(buf, bufsize, "%c", dateval);

  return (buf);
}

static ipp_t* cups_new_request(const char* printer ,const char* which ,const char* users)
{
    static const char *jattrs[] =		/* Attributes we need for jobs... */
          {
            "copies",
            "job-id",
            "job-k-octets",
            "job-name",
            "job-originating-user-name",
            "job-printer-state-message",
            "job-printer-uri",
            "job-state",
            "job-state-reasons",
            "time-at-creation",
            "time-at-completed",
            "job-media-sheets-completed",
            "job-media-sheets",
            "document-name",
          };

    ipp_t		*request;		/* IPP Request */
    request = ippNewRequest(IPP_GET_JOBS);

    if(!which)
        which = "all";

    if(printer){
        char uri[256];
        httpAssembleURIf(HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp", NULL, "localhost", ippPort(), "/printers/%s", printer);

        ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
                     NULL, uri);
    }else{
        ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
                     NULL, "ipp://localhost/");
    }

    ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
                  "requested-attributes", sizeof(jattrs) / sizeof(jattrs[0]),
          NULL, jattrs);

  //  ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
  //               NULL, cupsUser());

    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD, "which-jobs",
                 NULL, which);

    return request;
}



static int cups_get_job_with_request(CALLBACK_getJob callback,void* para,ipp_t *request)
{

    Job_struct job_s ,*js = &job_s;

const char* dests = NULL;
  ipp_t
        *response;		/* IPP Response */
  ipp_attribute_t *attr,		/* Current attribute */
        *reasons;		/* Job state reasons attribute */
  const char	*dest,			/* Pointer into job-printer-uri */
        *username,		/* Pointer to job-originating-user-name */
        *message,		/* Pointer to job-printer-state-message */
        *time_at;		/* time-at-xxx attribute name to use */
  int
        jobid,			/* job-id */
        size;			/* job-k-octets */
  time_t	jobtime;		/* time-at-creation */
  int pages;
  int copies;
  ipp_jstate_t state;
  const char* jobname;

  response = cupsDoRequest(CUPS_HTTP_DEFAULT, request, "/");

  if (cupsLastError() == IPP_STATUS_ERROR_BAD_REQUEST ||
      cupsLastError() == IPP_STATUS_ERROR_VERSION_NOT_SUPPORTED)
  {
    LOGLOG(_("cups Error - add '/version=1.1' to server name."));
    ippDelete(response);
    return (1);
  }
  else if (cupsLastError() > IPP_STATUS_OK_CONFLICTING)
  {
    LOGLOG( "cups: %s", cupsLastErrorString());
    ippDelete(response);
    return (1);
  }

  if (response)
  {
   /*
    * Loop through the job list and display them...
    */

//    if (!strcmp(which, "aborted") ||
//        !strcmp(which, "canceled") ||
//        !strcmp(which, "completed"))
//      time_at = "time-at-completed";
//    else
      time_at = "time-at-creation";

    for (attr = response->attrs; attr != NULL; attr = attr->next)
    {
     /*
      * Skip leading attributes until we hit a job...
      */

        while (attr != NULL && attr->group_tag != IPP_TAG_JOB)
          attr = attr->next;

        if (attr == NULL)
          break;

       /*
        * Pull the needed attributes from this job...
        */

        jobid    = 0;
        size     = 0;
        username = NULL;
        dest     = NULL;
        jobtime  = 0;
        message  = NULL;
        reasons  = NULL;
        pages = 0;
        copies = 0;
        jobname = NULL;

        while (attr != NULL && attr->group_tag == IPP_TAG_JOB)
        {
            if (!strcmp(attr->name, "job-id") &&
                    attr->value_tag == IPP_TAG_INTEGER){
                jobid = attr->values[0].integer;

            }else if (!strcmp(attr->name, "job-k-octets") &&
                      attr->value_tag == IPP_TAG_INTEGER){
                size = attr->values[0].integer;

            }else if (!strcmp(attr->name, time_at) && attr->value_tag == IPP_TAG_INTEGER){
                jobtime = attr->values[0].integer;

            }
            else if (!strcmp(attr->name, "job-printer-state-message") &&
                     attr->value_tag == IPP_TAG_TEXT){
                message = attr->values[0].string.text;

            }
            else if (!strcmp(attr->name, "job-printer-uri") &&
                 attr->value_tag == IPP_TAG_URI)
            {
                if ((dest = strrchr(attr->values[0].string.text, '/')) != NULL)
                    dest ++;
            }
            else if (!strcmp(attr->name, "job-originating-user-name") &&
                     attr->value_tag == IPP_TAG_NAME){
                username = attr->values[0].string.text;

            }
            else if (!strcmp(attr->name, "job-state") &&
                     attr->value_tag == IPP_TAG_ENUM){
                state = (ipp_jstate_t)attr->values[0].integer;

            }
            else if (!strcmp(attr->name, "job-state-reasons") &&
                     attr->value_tag == IPP_TAG_KEYWORD){
                reasons = attr;
            }
            else if (!strcmp(attr->name, "job-media-sheets-completed") &&
                     attr->value_tag == IPP_TAG_INTEGER){
                pages = attr->values[0].integer;
            }
            else if (!strcmp(attr->name, "copies") &&
                     attr->value_tag == IPP_TAG_INTEGER){
                copies = attr->values[0].integer;
            }
            else if (!strcmp(attr->name, "job-name") &&
                     attr->value_tag == IPP_TAG_TEXT){
                jobname = attr->values[0].string.text;
                LOGLOG("job name:%s" ,jobname);
            }
            else if (!strcmp(attr->name, "document-name") &&
                     attr->value_tag == IPP_TAG_TEXT){
                LOGLOG("document-name:%s" ,attr->values[0].string.text);
            }
            else if (!strcmp(attr->name, "job-sheets-actual") &&
                     attr->value_tag == IPP_TAG_INTEGER){
                LOGLOG("job-sheets-actual:%d" ,attr->values[0].integer);
            }
            else if (!strcmp(attr->name, "job-media-sheets") &&
                     attr->value_tag == IPP_TAG_INTEGER){
                LOGLOG("job-media-sheets:%d" ,attr->values[0].integer);
            }

            attr = attr->next;
        }

     /*
      * See if we have everything needed...
      */

        if (dest == NULL || jobid == 0)
        {
          if (attr == NULL)
              break;
            else
              continue;
        }

     /*
      * Display the job...
      */

        if (match_list(dests, dest))
//      if (match_list(dests, dest) && match_list(users, username))
      {
            js->id = jobid;
            js->pages_completed = pages;
            js->document_size = size;
            js->copies = copies;
            strcpy(js->printer ,dest);
            js->timet = jobtime;
            jk_cupsStrDate(js->time, sizeof(js->time), jobtime);
            if(jobname){
                strcpy(js->name ,jobname);
            }else{
                strcpy(js->name ,"unknown");
            }
            if(username){
                strcpy(js->user_name ,username);
            }else{
                strcpy(js->user_name ,"unknown");
            }
            js->state = (int)state;
//            if(message)
//                strcpy(js->message ,message);

//            if (reasons)
//            {
//                char *aptr;		/* Pointer into alerts string */

//                for (i = 0, aptr = js->alerts; i < reasons->num_values; i ++)
//                {
//                    if (i){
//                        snprintf(aptr, sizeof(js->alerts) - (size_t)(aptr - js->alerts), " %s", reasons->values[i].string.text);
//                    }
//                    else{
//                        strncpy(js->alerts, reasons->values[i].string.text, sizeof(js->alerts));
////                        strcpy(js->alerts ,reasons->values[i].string.text);
//                    }

//                  aptr += strlen(aptr);
//                }

//            }
            callback(para ,js);
      }

      if (attr == NULL)
        break;
    }

    ippDelete(response);
  }

  return (0);
}

int cups_get_job(CALLBACK_getJob callback,void* para,const char* printer ,const char* which ,const char* users)
{
    ipp_t* request = cups_new_request(printer ,which ,users);
    return cups_get_job_with_request(callback ,para ,request);
}

#if 0
/*
 * 'timeout_cb()' - Handle HTTP timeouts.
 */

static int				/* O - 1 to continue, 0 to cancel */
timeout_cb(http_t *http,		/* I - Connection to server (unused) */
           void   *user_data)		/* I - User data (unused) */
{
  (void)http;
  (void)user_data;

  return 1;//(!job_canceled);
}

#include "backend/backend-private.h"
#include <sys/socket.h>
#include <netinet/in.h>
int snmpGetDeviceID(const char* device_uri ,char* buffer ,int bufsize)
{
    int ret = -1;

    char	hostname[1024];		/* Hostname */
    http_addr_t *addr;			/* I - Address of device */
    int		port;			/* Port number (not used) */
    char		portname[255];		/* Port name */
    http_addrlist_t *addrlist;		/* Address of printer */
    http_t	*http;			/* HTTP connection */

    char		scheme[255],		/* Scheme in URI */
          resource[1024];		/* Resource info (printer name) */
    char		username[256] = "";

    const char* resolved_uri;
    char uri[1024];
    resolved_uri = _httpResolveURI(device_uri, uri,
                              sizeof(uri), _HTTP_RESOLVE_DEFAULT, NULL, NULL);
    httpSeparateURI(HTTP_URI_CODING_ALL, resolved_uri, scheme, sizeof(scheme),
                    username, sizeof(username), hostname, sizeof(hostname), &port,
            resource, sizeof(resource));
    if(port < 0)
        port = IPP_PORT;
    sprintf(portname, "%d", port);
    addrlist = httpAddrGetList(hostname, AF_UNSPEC, portname);
//    if(CUPS_VERSION >= 1.7)
//        http = httpConnect2(hostname, port, addrlist, AF_UNSPEC, cupsEncryption(), 1,
//                        0, NULL);
//    else
        http = httpConnect(hostname ,port);
        if(!http)
            return -1;
    httpSetTimeout(http, 30.0, timeout_cb, NULL);
    addr = http->hostaddr;

    int snmp_fd;
    snmp_fd = _cupsSNMPOpen(addrlist->addr.addr.sa_family);

    if (snmp_fd >= 0)
    {
        cups_snmp_t	packet;		/* Packet from printer */
        static const int ppmPrinterIEEE1284DeviceId[] =
              { CUPS_OID_ppmPrinterIEEE1284DeviceId,1,-1 };

        if (_cupsSNMPWrite(snmp_fd, addr, CUPS_SNMP_VERSION_1,
                       _cupsSNMPDefaultCommunity(),
                       CUPS_ASN1_GET_REQUEST, 1,
               ppmPrinterIEEE1284DeviceId))
        {
            if (_cupsSNMPRead(snmp_fd, &packet, 1.0) &&
                packet.object_type == CUPS_ASN1_OCTET_STRING)
            {
              strncpy(buffer, (char *)packet.object_value.string.bytes,
                      bufsize);
              buffer[bufsize - 1] = 0;
//              strcpy(buffer, (char *)packet.object_value.string.bytes)
              ret = 0;
            }
        }
        _cupsSNMPClose(snmp_fd);
    }
    httpClose(http);
    return ret;
}
#endif
int cups_resolve_uri(const char* device_uri ,char* buffer ,int bufsize)
{
    const char* resolved_uri;
    resolved_uri = _httpResolveURI(device_uri, buffer,
                              bufsize, _HTTP_RESOLVE_DEFAULT, NULL, NULL);
    return resolved_uri != NULL ?0 :-1;
}


int get_device_model(const char* printer ,char* model)
{
    cups_dest_t *dests;
    int num_dests;
    num_dests = cupsGetDests(&dests);
    cups_dest_t* dest;
    dest = cupsGetDest(printer ,NULL ,num_dests ,dests);
    if(!dest)
        return NULL;
    strcpy(model ,cupsGetOption("printer-make-and-model", dest->num_options, dest->options));
    cupsFreeDests(num_dests ,dests);
    return 0;
}

int cups_get_device_uri(const char* printer ,char* device_uri)
{
    cups_dest_t *dests;
    int num_dests;
    num_dests = cupsGetDests(&dests);
    cups_dest_t* dest;
    dest = cupsGetDest(printer ,NULL ,num_dests ,dests);
    if(!dest)
        return NULL;
    strcpy(device_uri ,cupsGetOption("device-uri", dest->num_options, dest->options));
    cupsFreeDests(num_dests ,dests);
    return 0;
}

const char* cups_get_default_printer()
{
    return cupsGetDefault();
}

int cups_get_printers(CALLBACK_getPrinters callback ,void* para)
{
    if(!callback)
        return -2;
    int ret = 0;
    Printer_struct printer ,*pp;
    pp = &printer;

    cups_dest_t *dests;
    int num_dests;
    num_dests = cupsGetDests(&dests);
    cups_dest_t *dest;
    int i;
    const char *value;
    const char* device_uri;

    char		scheme[255],		/* Scheme in URI */
          hostname[1024],		/* Hostname */
          resource[1024];		/* Resource info (printer name) */
    char		username[256] = "";
    int port;
   for (i = num_dests, dest = dests; i > 0; i --, dest ++)
   {
       if (dest->instance == NULL){

       }
       strcpy(pp->name ,dest->name);
//       device_uri = _cupsGetDestResource(dest ,resource ,sizeof(resource));
       device_uri =  cupsGetOption("device-uri", dest->num_options, dest->options);
       strcpy(pp->deviceUri ,device_uri);
       value = cupsGetOption("printer-make-and-model", dest->num_options, dest->options);
       strcpy(pp->makeAndModel ,value);
       pp->isDefault = dest->is_default;

       httpSeparateURI(HTTP_URI_CODING_ALL, device_uri, scheme, sizeof(scheme),
                       username, sizeof(username), hostname, sizeof(hostname), &port,
               resource, sizeof(resource));
//       httpSeparateURI(HTTP_URI_CODING_ALL, device_uri, pp->scheme, sizeof(pp->scheme),
//                       pp->username, sizeof(pp->username), pp->hostname, sizeof(pp->hostname), &pp->port,
//               pp->resource, sizeof(pp->resource));
       if(!callback(para ,pp)){
           break;
       }
   }
   cupsFreeDests(num_dests, dests);
   return ret;
}

#include <cups/sidechannel.h>
int cups_usb_getDeviceID(char* _data ,int _datalen)
{
    int result;
    char data[1025];
    int datalen = 1024;
    cups_sc_status_t status;
    /* Tell cupsSideChannelDoRequest() how big our buffer is, less 1 byte for
       nul-termination... */
    datalen = sizeof(data) - 1;

    /* Get the IEEE-1284 device ID, waiting for up to 1 second */
    LOGLOG("filterlib cups: get device id");
    status = cupsSideChannelDoRequest(CUPS_SC_CMD_GET_DEVICE_ID, data, &datalen, 1.0);
    LOGLOG("filterlib cups: get device id %d: %s" ,datalen ,data);

    /* Use the returned value if OK was returned and the length is non-zero */
    if (status == CUPS_SC_STATUS_OK && datalen > 0)
    {
        data[datalen] = '\0';
        result = 0;
        if(_datalen > datalen){
            strcpy(_data ,data);
        }else{
            result = -1;
        }
    }
    else
    {
        data[0] = '\0';
        result = -1;
    }
    return result;
}

typedef struct device_struct{
    CALLBACK_getDevices func;
    void* para;
}
    device_struct;
static void
device_cb(
    const char *device_class,		/* I - device-class string */
    const char *device_id,		/* I - device-id string */
    const char *device_info,		/* I - device-info string */
    const char *device_make_and_model,	/* I - device-make-and-model string */
    const char *device_uri,		/* I - device-uri string */
    const char *device_location,	/* I - device-location string */
    void       *user_data)		/* I - User data */
{
  device_struct* data;
  data = (device_struct*) user_data;
  if(data && data->func){
      data->func(device_uri ,device_id ,data->para);
  }
  LOGLOG("Device: uri = %s\n"
                "        class = %s\n"
                "        info = %s\n"
                "        make-and-model = %s\n"
                "        device-id = %s\n"
                "        location = %s",
              device_uri, device_class, device_info,
              device_make_and_model, device_id, device_location);
}

#include "cups/adminutil.h"
int cups_getDevices(const char *include_schemes ,int timeout ,CALLBACK_getDevices func ,void* para)
{
    int ret = 0;
    device_struct data;
    data.func = func;
    data.para = para;
    if (cupsGetDevices(CUPS_HTTP_DEFAULT, timeout, include_schemes,
                       NULL, device_cb, (void*)&data) != IPP_OK)
    {
        ret = -1;
    }
    return ret;
}

int cups_get_printer_status(const char  *device_url)/* O - not busy, 1 busy */
{
    if(!device_url){
        LOGLOG("cups get printer url status:no device url!");
        return 0;
    }
  ipp_t		*request,		/* IPP Request */
        *response;		/* IPP Response */
  ipp_attribute_t *attr;		/* Current attribute */
  const char	*printer,		/* Printer name */
        *device;		/* Device URI */
  ipp_pstate_t	pstate;			/* Printer state */
  static const char *pattrs[] =		/* Attributes we need for printers... */
        {
          "device-uri",
          "printer-name",
          "printer-state",
        };

  int busy = 0;//not busy
//  if (printers != NULL && !strcmp(printers, "all"))
//    printers = NULL;

  request = ippNewRequest(CUPS_GET_PRINTERS);

  ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
                "requested-attributes", sizeof(pattrs) / sizeof(pattrs[0]),
        NULL, pattrs);

//  ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
//               NULL, cupsUser());

  response = cupsDoRequest(CUPS_HTTP_DEFAULT, request, "/");

  if (cupsLastError() == IPP_STATUS_ERROR_BAD_REQUEST ||
      cupsLastError() == IPP_STATUS_ERROR_VERSION_NOT_SUPPORTED)
  {
    LOGLOG("Error - add '/version=1.1' to server name.");
    ippDelete(response);
    return (1);
  }
  else if (cupsLastError() > IPP_STATUS_OK_CONFLICTING)
  {
    LOGLOG("cups get printer: %s", cupsLastErrorString());
    ippDelete(response);
    return (1);
  }

  if (response)
  {

    for (attr = response->attrs; attr != NULL; attr = attr->next)
    {

      while (attr != NULL && attr->group_tag != IPP_TAG_PRINTER)
        attr = attr->next;

      if (attr == NULL)
        break;

      printer     = NULL;
      device    = "file:/dev/null";
      pstate      = IPP_PRINTER_IDLE;

      while (attr != NULL && attr->group_tag == IPP_TAG_PRINTER)
      {
          if (!strcmp(attr->name, "device-uri") &&
                  attr->value_tag == IPP_TAG_URI)
              device = attr->values[0].string.text;
          else if (!strcmp(attr->name, "printer-name") &&
                   attr->value_tag == IPP_TAG_NAME)
              printer = attr->values[0].string.text;
          else if (!strcmp(attr->name, "printer-state") &&
                   attr->value_tag == IPP_TAG_ENUM)
              pstate = (ipp_pstate_t)attr->values[0].integer;

        attr = attr->next;
      }


     /*
      * See if we have everything needed...
      */

      if ((printer == NULL) || (device == NULL))
      {
        if (attr == NULL)
            break;
        else
            continue;
      }
#if 0
      if(strncmp(device, "usb://", 6)){
          if (attr == NULL)
              break;
          else
              continue;
      }
      char _device_url[1024];
      strcpy(_device_url ,device);
      cups_resolve_uri(device ,_device_url ,1024);
      LOGLOG("cups get device:%s , %s",printer ,_device_url);
      if(!strcmp(device_url ,_device_url))
#else
//      LOGLOG("cups get device:%s , %s",printer ,device);
      if(!strcmp(device_url ,device))
#endif
//      if (match_list(printers, printer))
      {
        if (pstate == IPP_PRINTER_PROCESSING)
        {
            busy = 1;
            LOGLOG("cups printer: %s status is processing", printer);
            break;
        }
      }

      if (attr == NULL)
        break;
    }

    ippDelete(response);
  }

  return busy;
}
