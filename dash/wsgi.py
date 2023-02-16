from waitress import serve
from web_gui import app

serve(
    app.server,
    port=8888
)