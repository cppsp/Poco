//
// CodeWriter.cpp
//
// $Id: //poco/1.4/PageCompiler/src/CodeWriter.cpp#1 $
//
// Copyright (c) 2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "CodeWriter.h"
#include "Page.h"
#include "Poco/Path.h"
#include "Poco/StringTokenizer.h"


using Poco::Path;
using Poco::StringTokenizer;


CodeWriter::CodeWriter(const Page& page, const std::string& clazz):
	_page(page),
	_class(clazz)
{
}


CodeWriter::~CodeWriter()
{
}


void CodeWriter::writeHeader(std::ostream& ostr, const std::string& headerFileName)
{
	beginGuard(ostr, headerFileName);
	writeHeaderIncludes(ostr);
	ostr << "\n\n";
	
	std::string decls(_page.headerDecls().str());
	if (!decls.empty())
	{
		ostr << decls << "\n\n";
	}
	
	beginNamespace(ostr);
	writeHandlerClass(ostr);
	writeFactoryClass(ostr);
	endNamespace(ostr);
	endGuard(ostr, headerFileName);
}


void CodeWriter::writeImpl(std::ostream& ostr, const std::string& headerFileName)
{
	ostr << "#include \"" << headerFileName << "\"\n";
	writeImplIncludes(ostr);
	if (_page.getBool("page.buffered", false))
	{
		ostr << "#include \"Poco/StreamCopier.h\"\n";
		ostr << "#include <sstream>\n";
	}
	ostr << "\n\n";

	std::string decls(_page.implDecls().str());
	if (!decls.empty())
	{
		ostr << decls << "\n\n";
	}

	beginNamespace(ostr);
	writeConstructor(ostr);
	writeHandler(ostr);
	writeFactory(ostr);
	endNamespace(ostr);
	writeManifest(ostr);
}


void CodeWriter::beginNamespace(std::ostream& ostr)
{
	std::string ns = _page.get("page.namespace", "");
	if (!ns.empty())
	{
		StringTokenizer tok(ns, ":", StringTokenizer::TOK_IGNORE_EMPTY | StringTokenizer::TOK_TRIM);
		for (StringTokenizer::Iterator it = tok.begin(); it != tok.end(); ++it)
		{
			ostr << "namespace " << *it << " {\n";
		}
		ostr << "\n\n";
	}
}

void CodeWriter::endNamespace(std::ostream& ostr)
{
	std::string ns = _page.get("page.namespace", "");
	if (!ns.empty())
	{
		ostr << "\n\n";
		StringTokenizer tok(ns, ":", StringTokenizer::TOK_IGNORE_EMPTY | StringTokenizer::TOK_TRIM);
		for (StringTokenizer::Iterator it = tok.begin(); it != tok.end(); ++it)
		{
			ostr << "} ";
		}
		ostr << "// namespace " << ns << "\n";
	}
}


void CodeWriter::beginGuard(std::ostream& ostr, const std::string& headerFileName)
{
	Path p(headerFileName);
	std::string guard(p.getBaseName());
	guard += "_INCLUDED";
	ostr << "#ifndef " << guard << "\n";
	ostr << "#define " << guard << "\n";
	ostr << "\n\n";
}


void CodeWriter::endGuard(std::ostream& ostr, const std::string& headerFileName)
{
	Path p(headerFileName);
	std::string guard(p.getBaseName());
	guard += "_INCLUDED";
	ostr << "\n\n";
	ostr << "#endif // " << guard << "\n";
}


void CodeWriter::handlerClass(std::ostream& ostr, const std::string& base, const std::string& ctorArg)
{
	std::string exprt(_page.get("page.export", ""));
	if (!exprt.empty()) exprt += ' ';

	ostr << "class " << exprt << _class << ": public " << base << "\n";
	ostr << "{\n";
	ostr << "public:\n";
	if (!ctorArg.empty())
	{
		ostr << "\t" << _class << "(" << ctorArg << ");\n";
		ostr << "\n";
	}
	ostr << "\tvoid handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);\n";
	writeHandlerMembers(ostr);
	ostr << "};\n";
}


void CodeWriter::factoryClass(std::ostream& ostr, const std::string& base)
{
	ostr << "class " << _class << "Factory: public " << base << "\n";
	ostr << "{\n";
	ostr << "public:\n";
	ostr << "\tPoco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);\n";
	ostr << "};\n";
}


void CodeWriter::factoryImpl(std::ostream& ostr, const std::string& arg)
{
	ostr << "Poco::Net::HTTPRequestHandler* " << _class << "Factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)\n";
	ostr << "{\n";
	ostr << "\treturn new " << _class << "(" << arg << ");\n";
	ostr << "}\n";
}


void CodeWriter::writeHeaderIncludes(std::ostream& ostr)
{
	ostr << "#include \"Poco/Net/HTTPRequestHandler.h\"\n";
}


void CodeWriter::writeHandlerClass(std::ostream& ostr)
{
	std::string base(_page.get("page.baseClass", "Poco::Net::HTTPRequestHandler"));
	std::string ctorArg;
	ctorArg = _page.get("page.context", _page.get("page.ctorArg", ""));

	handlerClass(ostr, base, ctorArg);
}


void CodeWriter::writeHandlerMembers(std::ostream& ostr)
{
	std::string context(_page.get("page.context", ""));
	if (!context.empty())
	{
		ostr << "\n";
		ostr << "protected:\n";
		ostr << "\t" << context << " context() const\n";
		ostr << "\t{\n";
		ostr << "\t\treturn _context;\n";
		ostr << "\t}\n";
		ostr << "\n";
		ostr << "private:\n";
		ostr << "\t" << context << " _context;\n";
	}
}


void CodeWriter::writeFactoryClass(std::ostream& ostr)
{
}


void CodeWriter::writeImplIncludes(std::ostream& ostr)
{
	ostr << "#include \"Poco/Net/HTTPServerRequest.h\"\n";
	ostr << "#include \"Poco/Net/HTTPServerResponse.h\"\n";
	ostr << "#include \"Poco/Net/HTMLForm.h\"\n";
}


void CodeWriter::writeConstructor(std::ostream& ostr)
{
	std::string base(_page.get("page.baseClass", "Poco::Net::HTTPRequestHandler"));
	std::string context(_page.get("page.context", ""));
	std::string ctorArg(_page.get("page.ctorArg", ""));
	if (!context.empty())
	{
		ostr << _class << "::" << _class << "(" << context << " context):\n";
		ostr << "\t_context(context)\n";
		ostr << "{\n}\n";
		ostr << "\n\n";
	}
	else if (!ctorArg.empty())
	{
		ostr << _class << "::" << _class << "(" << ctorArg << " arg):\n";
		ostr << "\t" << base << "(arg)\n";
		ostr << "{\n}\n";
		ostr << "\n\n";
	}
}


void CodeWriter::writeHandler(std::ostream& ostr)
{
	ostr << "void " << _class << "::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)\n";
	ostr << "{\n";
	writeResponse(ostr);
	if (_page.has("page.precondition"))
	{
		ostr << "\tif (!(" << _page.get("page.precondition") << ")) return;\n\n";
	}
	writeSession(ostr);
	writeForm(ostr);
	ostr << _page.preHandler().str();
	writeContent(ostr);
	ostr << "}\n";
}


void CodeWriter::writeFactory(std::ostream& ostr)
{
}


void CodeWriter::writeManifest(std::ostream& ostr)
{
}


void CodeWriter::writeSession(std::ostream& ostr)
{
}


void CodeWriter::writeForm(std::ostream& ostr)
{
	if (_page.getBool("page.form", true))
	{
		std::string partHandler(_page.get("page.formPartHandler", ""));
		if (!partHandler.empty())
		{
			ostr << "\t" << partHandler << " cpspPartHandler(*this);\n";
		}
		ostr << "\tPoco::Net::HTMLForm form(request, request.stream()";
		if (!partHandler.empty())
		{
			ostr << ", cpspPartHandler";
		}
		ostr << ");\n";
	}
}


void CodeWriter::writeResponse(std::ostream& ostr)
{
	std::string contentType(_page.get("page.contentType", "text/html"));
	bool buffered(_page.getBool("page.buffered", false));
	bool chunked(_page.getBool("page.chunked", !buffered));

	if (chunked)
	{
		ostr << "\tresponse.setChunkedTransferEncoding(true);\n";
	}

	ostr << "\tresponse.setContentType(\"" << contentType << "\");\n";
	ostr << "\n";
}


void CodeWriter::writeContent(std::ostream& ostr)
{
	bool buffered(_page.getBool("page.buffered", false));
	bool chunked(_page.getBool("page.chunked", !buffered));
	
	if (buffered)
	{
		ostr << "\tstd::stringstream responseStream;\n";
		ostr << _page.handler().str();
		if (!chunked)
		{
			ostr << "\tresponse.setContentLength(static_cast<int>(responseStream.tellp()));\n";
		}
		ostr << "\tPoco::StreamCopier::copyStream(responseStream, response.send());\n";		
	}
	else
	{
		ostr << "\tstd::ostream& responseStream = response.send();\n";
		ostr << _page.handler().str();
	}
}
