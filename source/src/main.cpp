#include "main.h"
#include <memory>

//std::string url="https://192.168.243.129/first.html";
////std::string url="https://jsonplaceholder.typicode.com/posts/1/comments";
////std::string url="http://sg1-l8rs0bg2.emrsn.org:5000/home/api";
////std::string url="https://httpbin.org/get";

int main()
{
	int x = 0;
	std::unique_ptr<RestAPIFacade> rest = nullptr;

	try
	{
		rest = std::unique_ptr<RestAPIFacade>(RestAPIFacade::GetInstance("192.168.243.129/api/"));
	}

	catch(std::exception & e)
	{

		std::cout<<"\n Exception - "<<e.what()<<std::endl;
		exit(1);
	}

//	Certificate cert = {};
//	cert.strPassword = "Panch@123";
//	cert.strCertificate = "/home/developer/opc/work/CA/clientcert.pem";
//	cert.strCAcertificate = "/home/developer/opc/work/CA/cacert.pem";
//	cert.strKeyName = "/home/developer/opc/work/CA/clientkey.pem";
//	cert.strKeyType = "PEM";
//	rest->SetCertificateAttributes(cert);

	rest->CreateDeviceData("v2/general/private/post",RestApiData());
	//rest->GetDeviceData("");

	std::cout<<"\n Press Enter to continue!!! "<<std::endl;
	std::cin >> x;

	return 0;
}

//static size_t function(char * buf, size_t size, size_t nmemb, void * up);
//static void dump(const char *text,FILE *stream, unsigned char *ptr, size_t size);
//static int my_trace(CURL *handle, curl_infotype type,char *data, size_t size,void *userp);
//static CURLcode sslctx_function(CURL *curl, void *sslctx, void *parm);

//int main(void)
//{
//  CURL *curl;
//  CURLcode res;
//  FILE *headerfile;
//  const char *pPassphrase = "Panch@123";
//
//#ifdef USE_ECC
//  static const char *pCertFile = "/home/developer/opc/work/CA/ecc/client/client.pem";//"testcert.pem";
//  static const char *pCACertFile= "/home/developer/opc/work/CA/cacert.pem";//"cacert.pem";
//  static const char *pHeaderFile = "dumpit";
//#else
//  static const char *pCertFile = "/home/developer/opc/work/CA/clientcert.pem";//"testcert.pem";
//  static const char *pCACertFile= "/home/developer/opc/work/CA/cacert.pem";//"cacert.pem";
//  static const char *pHeaderFile = "dumpit";
//#endif
//
//  const char *pKeyName;
//  const char *pKeyType;
//
//  const char *pEngine;
//
//#ifdef USE_ENGINE
//  pKeyName  = "rsa_test";
//  pKeyType  = "ENG";
//  pEngine   = "chil";            /* for nChiper HSM... */
//#else
//#ifdef USE_ECC
//  pKeyName  = "/home/developer/opc/work/CA/ecc/client/private-key.pem";
//#else
//  pKeyName  = "/home/developer/opc/work/CA/clientkey.pem";//"testkey.pem";
//#endif
//  pKeyType  = "PEM";
//  pEngine   = NULL;
//#endif
//
//  headerfile = fopen(pHeaderFile, "wb");
//
//  curl_global_init(CURL_GLOBAL_DEFAULT);
//
//  curl = curl_easy_init();
//
//  if(curl) {
//    /* what call to write: */
//    curl_easy_setopt(curl, CURLOPT_URL, "https://192.168.243.129/first.html");
//    curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfile);
//
//    do { /* dummy loop, just to break out from */
//      if(pEngine) {
//        /* use crypto engine */
//        if(curl_easy_setopt(curl, CURLOPT_SSLENGINE, pEngine) != CURLE_OK) {
//          /* load the crypto engine */
//          fprintf(stderr, "can't set crypto engine\n");
//          break;
//        }
//        if(curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L) != CURLE_OK) {
//          /* set the crypto engine as default */
//          /* only needed for the first time you load
//             a engine in a curl object... */
//          fprintf(stderr, "can't set crypto engine as default\n");
//          break;
//        }
//      }
//      /* cert is stored PEM coded in file... */
//      /* since PEM is default, we needn't set it for PEM */
//      curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
//
//      /* set the cert for client authentication */
//      curl_easy_setopt(curl, CURLOPT_SSLCERT, pCertFile);
//
//      /* sorry, for engine we must set the passphrase
//         (if the key has one...) */
//      if(pPassphrase)
//        curl_easy_setopt(curl, CURLOPT_KEYPASSWD, pPassphrase);
//
//      /* if we use a key stored in a crypto engine,
//         we must set the key type to "ENG" */
//      curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, pKeyType);
//
//      /* set the private key (file or ID in engine) */
//      curl_easy_setopt(curl, CURLOPT_SSLKEY, pKeyName);
//
//      /* set the file with the certs vaildating the server */
//      curl_easy_setopt(curl, CURLOPT_CAINFO, pCACertFile);
//
//      /* disconnect if we can't validate server's cert */
//      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
//
//      /* Perform the request, res will get the return code */
//      res = curl_easy_perform(curl);
//      /* Check for errors */
//      if(res != CURLE_OK)
//        fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                curl_easy_strerror(res));
//
//      /* we are done... */
//    } while(0);
//    /* always cleanup */
//    curl_easy_cleanup(curl);
//  }
//
//  curl_global_cleanup();
//}

//static CURLcode sslctx_function(CURL *curl, void *sslctx, void *parm)
//{
//	std::cout<<"\n sslctx_function called"<<std::endl;
//  /* all set to go */
//  return CURLE_OK;
//}


//std::string url="https://192.168.243.129/first.html";
////std::string url="https://jsonplaceholder.typicode.com/posts/1/comments";
////std::string url="http://sg1-l8rs0bg2.emrsn.org:5000/home/api";
////std::string url="https://httpbin.org/get";
//std::string urldata="";
//
//CURL * curl = nullptr;
//
//int main()
//{
//
//	//std::string userpwd="mrpanchu2003@gmail.com:NumeroUno@123";
//    curl = curl_easy_init();
////    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
////    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &function);
//
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
//    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//    /* require use of SSL for this, or fail */
//    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
//    CURLcode res = curl_easy_perform(curl);
//    if (res == CURLE_OK)
//    {
//        std::cout<<"\n CURL Success"<<std::endl;
//        std::cout<<"\n The data read is : \n"<<urldata<<std::endl;
//    }
//    else
//    {
//        std::cout<<"\n CURL Failed"<<std::endl;
//    }
//    curl_easy_cleanup(curl);
//    return 0;
//}

//
//size_t function(char * buf, size_t size, size_t nmemb, void * up)
//{
//    std::cout << "\n function Called" << std::endl;
//    urldata = std::string(buf);
//    return size*nmemb;
//}

//static
//void dump(const char *text,
//          FILE *stream, unsigned char *ptr, size_t size)
//{
//  size_t i;
//  size_t c;
//  unsigned int width=0x10;
//
//  fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
//          text, (long)size, (long)size);
//
//  for(i=0; i<size; i+= width) {
//    fprintf(stream, "%4.4lx: ", (long)i);
//
//    /* show hex to the left */
//    for(c = 0; c < width; c++) {
//      if(i+c < size)
//        fprintf(stream, "%02x ", ptr[i+c]);
//      else
//        fputs("   ", stream);
//    }
//
//    /* show data on the right */
//    for(c = 0; (c < width) && (i+c < size); c++) {
//      char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
//      fputc(x, stream);
//    }
//
//    fputc('\n', stream); /* newline */
//  }
//}
//
//static
//int my_trace(CURL *handle, curl_infotype type,
//             char *data, size_t size,
//             void *userp)
//{
//  const char *text;
//  (void)handle; /* prevent compiler warning */
//  (void)userp;
//
//  switch (type) {
//  case CURLINFO_TEXT:
//    fprintf(stderr, "== Info: %s", data);
//  default: /* in case a new one is introduced to shock us */
//    return 0;
//
//  case CURLINFO_HEADER_OUT:
//    text = "=> Send header";
//    break;
//  case CURLINFO_DATA_OUT:
//    text = "=> Send data";
//    break;
//  case CURLINFO_SSL_DATA_OUT:
//    text = "=> Send SSL data";
//    break;
//  case CURLINFO_HEADER_IN:
//    text = "<= Recv header";
//    break;
//  case CURLINFO_DATA_IN:
//    text = "<= Recv data";
//    break;
//  case CURLINFO_SSL_DATA_IN:
//    text = "<= Recv SSL data";
//    break;
//  }
//
//  dump(text, stderr, (unsigned char *)data, size);
//  return 0;
//}
