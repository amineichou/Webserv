# Webserv

> "This is when you finally understand why a URL starts with HTTP"

## 📖 Project Overview

**Webserv** is a lightweight HTTP/1.1 web server implemented in C++98, developed as a part of the 42 school curriculum. It is capable of handling multiple clients simultaneously using non-blocking I/O with `poll()` (or its equivalents), serving static content, executing CGI scripts, supporting multiple HTTP methods (GET, POST, DELETE), and simulating essential features found in popular web servers like NGINX.

---

## 🔧 Features

- ✅ Compatible with HTTP/1.1
- ✅ Non-blocking I/O using `poll()` (or `select`, `kqueue`, `epoll`)
- ✅ Support for multiple client connections
- ✅ Handles GET, POST, and DELETE methods
- ✅ Static file serving
- ✅ CGI support (e.g., PHP, Python)
- ✅ File upload support via POST
- ✅ Multiple server blocks & virtual hosting
- ✅ Default error pages
- ✅ Directory listing (optional)
- ✅ Redirections
- ✅ Configuration file inspired by NGINX
- ✅ Stress-tested for resilience
- ✅ Compliant with C++98 standard

---

## ⚙️ Compilation

```bash
make
```

Makefile rules:

- make or make all: Build the server

- make clean: Remove object files

- make fclean: Remove object files and executable

- make re: Rebuild the entire project


Compiled with:

```bash
c++ -Wall -Wextra -Werror -std=c++98
```

## 🚀 Usage
```bash
./webserv [configuration_file]
```
If no configuration file is passed, a default one will be used.

## 🧾 Configuration
The configuration file is inspired by the NGINX style. You can:

- Define multiple server blocks with:

	- host and port

	- server_name

	- client_max_body_size

	- error_page definitions

- Define location (routes) blocks per server with:

	- Allowed HTTP methods

	- Root directory

	- Directory listing on/off

	- Default index files

	- Redirection rules

	- Upload destinations

	- CGI execution by extension


## 📂 Example Configuration

```conf
server [
    port = 3000;
    port = 127.1.1.1:8080;
    servernames = localhost;
    bodysizelimit = 3GB;
    globalroot = html;

    route / {
        autoindex = on;
        acceptedmethods = GET;
        index = index.html;
    }

    route /delete {
        autoindex =  off;
        index = delete.html;
        acceptedmethods = GET, DELETE;
    }

    route /login {
        # autoindex = on;
        acceptedmethods = GET, POST;
        index = login.py;
		root = html/login;
        cgiextension = php, /usr/bin/php;
        cgiextension = py, /usr/bin/python3;
        cgiextension = sh, /usr/bin/bash;
    }

    route /uploads {
        autoindex = on;
        acceptedmethods = GET, POST, DELETE;
        root = html/uploads;
    }

    route /uploading {
        autoindex = off;
        index = upload.html;
        uploaddir = uploads;
        acceptedmethods = GET, POST;
        root = html;  
    }

    route /about {
        autoindex = off;
        acceptedmethods = GET;
        return = 301, amineichou.uk;
    }
]
```

## 🧪 Testing

- Tested using browsers, curl, telnet, and Python scripts

- Compatible with major HTTP clients

- Compare behavior with NGINX for compliance


## 📝 Notes

- 🍪 Cookie and session management

- 🔁 Support for multiple CGI handlers (e.g., PHP & Python)

- No external libraries (including Boost) used

- No system calls like fork() allowed outside CGI

- read/write operations always go through poll() (or equivalent)

- MacOS requires fcntl() with specific flags only (F_SETFL, O_NONBLOCK, FD_CLOEXEC)

- Compiled and tested on a linux envirement (Linux 5.15.0-136-generic #147-Ubuntu SMP Sat Mar 15 15:53:30 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux)




end.
