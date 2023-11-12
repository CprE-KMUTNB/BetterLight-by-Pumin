// light bulb -> {"action":"on","brightness":137,"color":{"x":0.3131,"y":0.3232},"color_mode":"color_temp","color_temp":153,"linkquality":126,"state":"ON"}
// button -> {"action":"1_single"}

const webSocket = new WebSocket('ws://betterlight.local:443/')
webSocket.addEventListener("open", () => {console.log("We are connected")})
webSocket.onmessage = (event) => {reload(event)}
webSocket.addEventListener("error", (e) => {console.log(e)})

function onLoad() {
    const default_status = {
        state: "ON",
        color: '#00FF00',
        brightness: 86,
        color_temp: 154
    }
    const template = document.getElementById('report-temp').content.querySelector("div")
    for (let i=0; i < 2; i++) {
        let item = document.importNode(template, true)
        item.id = `light-bulb-${i+1}`
        item.getElementsByTagName("B")[0].innerHTML = `Light Bulb ${i+1}`
        const status = item.getElementsByTagName("SPAN")
        status[0].innerHTML = `State: ${default_status.state}`
        status[1].innerHTML = `Color:<input type="color" id="color${i+1}" value="${default_status.color}"> ${default_status.color}`
        status[2].innerHTML = `Brightness: ${default_status.brightness}`
        status[3].innerHTML = `Color Temperature: ${default_status.color_temp}`
        item.getElementsByTagName("IMG")[0].src = `Assets/Light_${default_status.state}.png`
        document.getElementById("status").appendChild(item)
    }
}

function reload(event) {
    const light_status = data_filter(event.data)
    const parent_length = document.getElementById("status").children.length

    for (let i = 0; i < parent_length; i++) {
        try {
            const report = document.getElementById(light_status[i].name)
            const status = report.getElementsByTagName("SPAN")
            status[0].innerHTML = `State: ${light_status[i].state}`
            status[1].innerHTML = `Color:<input type="color" id="color${i+1}" value="${light_status[i].color}"> ${light_status[i].color}`
            status[2].innerHTML = `Brightness: ${light_status[i].brightness}`
            status[3].innerHTML = `Color Temperature: ${light_status[i].color_temp}`
            report.getElementsByTagName("IMG")[0].src = `Assets/Light_${light_status[i].state}.png`
        } catch(err) {
            continue
        }
    }

    function data_filter(IN) {
        IN = JSON.parse(IN)
        let OUT = []
        for (let i = 0; i < IN.length; i++) {
            if (typeof(IN[i].color.x) != "undefined" && typeof(IN[i].color.y) != "undefined") {
                color_in = xyToSRGB(IN[i].color.x, IN[i].color.y)
            }
            color_in = rgbToHex(color_in.r, color_in.g, color_in.b).toUpperCase()
            obj = {
                name: IN[i].name,
                state: IN[i].state,
                color: color_in,
                brightness: IN[i].brightness,
                color_temp: IN[i].color_temp
            }
            OUT.push(obj)
        }
        return OUT
    }

    function rgbToHex(r, g, b) {
        return "#" + (1 << 24 | r << 16 | g << 8 | b).toString(16).slice(1);
    }

    function xyToSRGB(x, y) {
        // Assuming D65 standard illuminant
        var Y = 1.0;
        var X = (Y / y) * x;
        var Z = (Y / y) * (1 - x - y);
    
        // Convert to sRGB using a transformation matrix
        var R = X * 3.2406 + Y * -1.5372 + Z * -0.4986;
        var G = X * -0.9689 + Y * 1.8758 + Z * 0.0415;
        var B = X * 0.0557 + Y * -0.2040 + Z * 1.0570;
    
        // Apply gamma correction
        R = R <= 0.0031308 ? 12.92 * R : 1.055 * Math.pow(R, 1 / 2.4) - 0.055;
        G = G <= 0.0031308 ? 12.92 * G : 1.055 * Math.pow(G, 1 / 2.4) - 0.055;
        B = B <= 0.0031308 ? 12.92 * B : 1.055 * Math.pow(B, 1 / 2.4) - 0.055;
    
        // Clamp values to the range [0, 1]
        R = Math.min(1, Math.max(0, R));
        G = Math.min(1, Math.max(0, G));
        B = Math.min(1, Math.max(0, B));
    
        // Convert to 8-bit integer values
        R = Math.round(R * 255);
        G = Math.round(G * 255);
        B = Math.round(B * 255);
    
        return { r: R, g: G, b: B };
    }
}

function changeMode(mode) {
    // console.log(mode);
    if (mode == 0) {
        webSocket.send('{"action":"1_single"}')
    } else if (mode == 1) {
        webSocket.send('{"action":"2_single"}')
    } else if (mode == 2) {
        webSocket.send('{"action":"3_single"}')
    } else if (mode == 3) {
        webSocket.send('{"action":"4_single"}')
    } else if (mode == 4) {
        webSocket.send('{"action":"1_double"}')
    } else if (mode == 5) {
        webSocket.send('{"action":"2_double"}')
    } else if (mode == 6) {
        webSocket.send('{"action":"3_double"}')
    } else if (mode == 7) {
        webSocket.send('{"action":"4_double"}')
    }
}
