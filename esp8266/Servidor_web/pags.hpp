


String p_inicio(){
    String pagina = 
    String("<html>")+
    "<head>"+
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"+
    "<style>"+
    "body{"+
    "background:#201f1f;"+
    "}"+
    ".OFF{"+
    "color:white;"+
    "background:red;"+
    "}"+
    ".ON{"+
    "color:white;"+
    "background:green;"+
    "}"+
    ".btn{"+
    "text-decoration: none;"+
    "padding: 10px;"+
    "margin: 10px;"+
    "border: 1px solid;"+
    "border-radius: 5px;"+
    "box-shadow: inset 0px -4px 10px black;"+
    "}"+
    ".btn:hover{"+
    "box-shadow: inset 1px 0px 10px 7px #000000d1;"+
    "}"+
    ".container{"+
    "text-align:center;"+
    "}"+
    ".btn-cont{"+
    "margin:40px;"+
    "}"+
    "span{"+
    "font-weight: bold;"+
    "color:white;"+
    "}"+
    "</style>"+
    "</head>"+
    "<body>"+
    "<div class='container'>"+
    "<div class='btn-cont'>"+
    "<span>Led 1</span><a class='btn "+clase1+"' href='/action?led=1'>"+clase1+
    "</a>"+
    "</div>"+
    "<div>"+
    "<span>Led 2</span><a class='btn "+clase2+"' href='/action?led=2'>"+clase2+
    "</a>"+
    "</div>"+
    "</div>"+
    "</body>"+
    "</html>";

    return pagina;
  
}
