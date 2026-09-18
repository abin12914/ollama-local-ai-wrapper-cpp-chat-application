# AI-Chat-Local

> **This application was fully generated with the assistance of AI.** The source code, user interface, asynchronous request flow, settings system, Markdown renderer, build configuration, and documentation were produced and iteratively refined through AI-assisted development.

AI-Chat-Local is a native Linux desktop chat application written in C++17. It provides a GTK 3 interface for communicating with locally installed language models through Ollama.

## Ollama Dependency

This application depends on [Ollama](https://ollama.com/) for model management and text generation. AI-Chat-Local does not contain or run an AI model by itself.

Ollama must be installed and available on the local machine. The application communicates with the default Ollama HTTP API at:

```text
http://localhost:11434
```

The application uses these Ollama operations:

- `GET /api/tags` to check whether Ollama is available and list installed models
- `POST /api/generate` to send prompts and receive generated responses
- `POST /api/stop` to unload the selected model

### Custom Ollama endpoints

The default endpoint is:

```text
http://localhost:11434
```

You can connect to another Ollama host from **Settings** by changing the
**Ollama endpoint** field. Enter the base URL without an API path, for example:

```text
http://192.168.1.20:11434
https://ollama.example.com
```

The application appends `/api/tags`, `/api/generate`, and `/api/stop` as needed.
Trailing slashes are removed automatically. The endpoint is saved in the user
settings file and restored when the application starts.

At least one Ollama model must be installed before sending a chat message. For example:

```bash
ollama pull llama3.2:1b
```

Start Ollama using the method appropriate for your installation:

```bash
ollama serve
```

If Ollama is installed as a system service, the application can also request service start and stop operations through the user interface. Those operations depend on `systemctl` permissions and the local Ollama service configuration.

## Features

- Native GTK 3 desktop interface
- Local model selection through Ollama
- Ollama service status indicator and switch
- Asynchronous AI requests using a background worker thread
- Multi-turn conversation context sent with each request
- User-configurable conversation context length
- Responsive interface while a model is generating a response
- Animated `Thinking` indicator during generation
- Streaming response assembly into one AI message
- `None` model option for unloading the current model
- User, AI, and system messages with separate sender and body colors
- Separate system colors for info, warning, error, and success messages
- Configurable window, chat background, chat input, input text, and cursor colors
- Persistent settings stored in the user configuration directory
- Basic Markdown rendering for AI responses
- Bundled application icon and Ubuntu desktop launcher installation
- Makefile-based build and installation workflow

## Markdown Rendering

AI responses support a lightweight Markdown subset rendered directly in the GTK text view:

- Headings using `#`, `##`, and similar heading markers
- Unordered lists using `*`, `-`, or `+`
- Ordered lists such as `1. Item`
- Bold text using `**bold**` or `__bold__`
- Italic text using `*italic*` or `_italic_`
- Inline code using backticks, for example `` `code` ``
- Paragraph line breaks

Example response:

```markdown
## 1. Performance

* C++ can execute close to the hardware.
* Compiled code can provide predictable performance.

**Important:** performance depends on the selected model and hardware.
```

The renderer is intentionally lightweight and is not a complete CommonMark implementation.

## Requirements

The following packages are required on Ubuntu or another GTK 3 Linux system:

- GNU C++ compiler with C++17 support
- GNU Make
- GTK 3 development files
- libcurl development files
- Ollama

On Ubuntu, install the build dependencies with:

```bash
sudo apt update
sudo apt install build-essential pkg-config libgtk-3-dev libcurl4-openssl-dev
```

Install Ollama separately using the official Ollama installation instructions, then pull at least one model.

## Building

From the project directory:

```bash
make clean
make
```

The output executable is:

```text
./ai-chat-local
```

The Makefile compiles these source files and links them into one executable:

- `main.cpp`: GTK window and widget construction
- `app_state.cpp`: shared application state definitions
- `AI-Chat-Local.cpp`: settings, chat rendering, Ollama communication, callbacks, and model/service logic
- `app_state.h`: shared state declarations
- `app.h`: application function declarations

The equivalent direct compiler command is:

```bash
g++ main.cpp app_state.cpp AI-Chat-Local.cpp \
    -o ai-chat-local \
    $(pkg-config --cflags --libs gtk+-3.0) \
    -lcurl
```

## Running

Build and run with Make:

```bash
make run
```

Or run the executable directly:

```bash
./ai-chat-local
```

The application should be started from a graphical Linux session with access to a display server and a running GTK environment.

## Installing for the Current User

The project includes a user-local installation target:

```bash
make install
```

This installs the application without requiring root privileges:

```text
~/.local/bin/ai-chat-local
~/.local/bin/app-icon.svg
~/.local/share/applications/ai-chat-local.desktop
~/.local/share/icons/hicolor/scalable/apps/ai-chat-local.svg
```

The desktop file allows AI-Chat-Local to appear in the Ubuntu application menu. If the launcher does not appear immediately, log out and back in, or refresh the desktop application cache if required by the desktop environment.

To remove the installed user-local files:

```bash
rm -f ~/.local/bin/ai-chat-local
rm -f ~/.local/bin/app-icon.svg
rm -f ~/.local/share/applications/ai-chat-local.desktop
rm -f ~/.local/share/icons/hicolor/scalable/apps/ai-chat-local.svg
```

## Settings

Settings are changed through the **Settings** button in the application window. Color selections are applied immediately and saved automatically to:

```text
~/.config/ai-chat-local/settings.ini
```

The settings include:

### Interface colors

- Window background color
- Ollama endpoint
- Chat background color
- Chat input background color
- Chat input text color
- Chat input cursor color, which follows the input text color

### Message colors

- User sender color
- User message color
- AI sender color
- AI response color
- System sender color
- System info color
- System warning color
- System error color
- System success color

If the settings file is removed, the application starts with its built-in default colors and recreates the file when a setting is changed.

## Architecture

The application uses a GTK main loop for all user-interface work and a GLib background thread for Ollama generation requests.

The main flow is:

1. GTK initializes the window and controls.
2. The application loads persisted colors.
3. Ollama availability is checked through its local API.
4. Installed models are loaded into the model selector.
5. A user message is inserted into the chat buffer.
6. A background worker sends the request to Ollama with libcurl.
7. The response fragments are assembled and queued back to the GTK main loop.
8. The GTK text buffer renders the AI response and applies Markdown text tags.
9. The thinking indicator is stopped after the response is complete.

### Conversation context

The application keeps recent user and assistant turns in memory. Before each
request, it sends the previous turns together with the new user message so the
model can answer with conversational context instead of seeing each prompt as
an isolated request. The history is currently bounded to the most recent 20
messages to limit prompt growth.

The **Conversation context messages** setting changes this limit. It defaults to
20 messages and accepts values from 1 to 100. Reducing the value immediately
removes the oldest turns beyond the new limit. The selected value is saved in
the user settings file and restored on startup.

Conversation history is cleared when the selected model changes or when the
current model is unloaded. It is not written to disk, so restarting the
application starts a new conversation.

Network calls must not be moved into the GTK event loop because they can block the interface while Ollama is busy.

## Monitoring Performance

Use `htop` while the application is running:

```bash
htop
```

Search for `ai-chat-local` with `F3`. Press `H` to toggle individual thread display.

Useful values include:

- `CPU%`: current CPU usage
- `MEM%`: percentage of system memory used
- `RES`: resident memory used by the application
- `TIME+`: accumulated CPU time
- `NLWP`: number of threads

Ollama normally consumes substantially more CPU and memory than this GTK client because the model inference runs in Ollama.

A command-line snapshot can be collected with:

```bash
watch -n 1 "ps -o pid,ppid,%cpu,%mem,rss,nlwp,etime,cmd -C ai-chat-local"
```

## Troubleshooting

### Ollama is not running

Start Ollama and confirm that its API responds:

```bash
ollama serve
```

Then check the API:

```bash
curl http://localhost:11434/api/tags
```

If Ollama is managed by systemd, check its status:

```bash
systemctl status ollama
```

### No model appears in the selector

Check that a model is installed:

```bash
ollama list
```

If necessary, pull one:

```bash
ollama pull llama3.2:1b
```

### The application cannot start under a Snap-based VS Code terminal

Some Snap environments can expose incompatible system libraries to native applications. A failure such as the following occurs before GTK starts:

```text
symbol lookup error: libpthread.so.0: undefined symbol: __libc_pthread_init
```

This is an environment/library mismatch rather than an application-level GTK error. Run the executable from a normal system terminal or a non-Snap environment to verify the GUI.

### The desktop icon does not appear immediately

Confirm that the files exist:

```bash
ls -l ~/.local/bin/ai-chat-local
ls -l ~/.local/share/applications/ai-chat-local.desktop
ls -l ~/.local/share/icons/hicolor/scalable/apps/ai-chat-local.svg
```

Restart the desktop session if the application menu has not refreshed.

## Cleaning Build Artifacts

Remove object files and the locally built executable with:

```bash
make clean
```

## Current Limitations

- The application currently targets GTK 3 and Linux desktop environments.
- Ollama must be installed separately and available on `localhost:11434`.
- Markdown support is a lightweight renderer, not a full Markdown parser.
- Prompts and response parsing are designed for the Ollama API behavior used by this application.
- The `systemctl` service controls require a compatible local service setup.
- Settings are stored per user and are not synchronized between machines.

## License and Contributions

This project is licensed under the [MIT License](LICENSE).

Copyright (c) 2026 iamab.in.

The MIT License permits use, copying, modification, distribution, sublicensing,
and sale of the software, provided that the copyright and permission notices
are included in copies or substantial portions of the software. The software is
provided without warranty.

Contributions can be made by improving the GTK interface, adding robust Markdown parsing, improving Ollama error reporting, adding automated tests, or extending packaging support.
