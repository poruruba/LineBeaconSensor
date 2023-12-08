'use strict';

const vConsole = new VConsole();
//const remoteConsole = new RemoteConsole("http://[remote server]/logio-post");
//window.datgui = new dat.GUI();

const CLOCK_UPDATE_INTERVAL = 20000;
const BRIGHTNESS_THRESHOLD = 4000;
const UDP_PORT = 12346;

const IMAGE_URL_BASE_LIST = [
    "https://unsplash.it/g/",
    "https://picsum.photos/",
    "https://p-hold.com/",
];

var vue_options = {
    el: "#top",
    mixins: [mixins_bootstrap],
    data: {
        datetime_now: new Date().getTime(),
        is_dark_prev: false,
        is_dark: false,
        base_url_index: 0,
    },
    computed: {
    },
    methods: {
        toDateString: function(tim){
            const d = new Date(tim);
            const weekStr = ["日", "月", "火", "水", "木", "金", "土"];
            return `${d.getFullYear()}年${d.getMonth() + 1}月${d.getDate()}日(${weekStr[d.getDay()]})`; 
        },
        toTimeString: function(tim){
            const to2d = (d) => {
                return ("00" + d).slice(-2);
            };
            const d = new Date(tim);
            return `${to2d(d.getHours())}:${to2d(d.getMinutes())}`; 
        },
        background_view_update: function(){
            if( !this.is_dark ){
              document.body.style.backgroundImage = 'url(' + this.background_url + ')';
            }else{
              document.body.style.backgroundImage = "none";
            }
        },
        background_image_change: async function(){
            console.log("background_change called");
            var width = document.documentElement.clientWidth;
            var height = document.documentElement.clientHeight;
            var base_url = IMAGE_URL_BASE_LIST[this.base_url_index];
            this.background_url = `${base_url}/${width}/${height}?p=${new Date().getTime()}`;
            console.log("url: " + this.background_url);
            this.background_view_update();
        },
        background_check_dark: function(is_dark_now){
            if( (this.is_dark == is_dark_now) || (is_dark_now && !this.is_dark_prev)){
              this.is_dark_prev = is_dark_now;
              return;
            }
            
            this.is_dark = is_dark_now;
            this.is_dark_perv = this.is_dark;
            this.background_view_update();
        },
        background_switch_base: async function(){
            this.base_url_index = ++this.base_url_index % IMAGE_URL_BASE_LIST.length;
            this.background_image_change();
        },
                  
        onDeviceReady: async function(){
            console.log("os: " + device.platform);

            window.powermanagement.acquire();

            simpleudp.receiving(UDP_PORT, packet =>{
                console.log(packet);
                var payload = JSON.parse(packet.payload);
                if( payload.type == 'sensor'){
                    this.background_check_dark(payload.brightness > BRIGHTNESS_THRESHOLD);
                }else
                if( payload.type == "ir" ){
                    this.background_switch_base();
                }
            });
        },
    },
    created: function(){
    },
    mounted: function(){
        proc_load();

        console.log("width: " + document.documentElement.clientWidth + " height: " + document.documentElement.clientHeight);
        
        this.background_image_change();

        setInterval(() =>{
            this.datetime_now = new Date().getTime();
        }, CLOCK_UPDATE_INTERVAL);
    }
};
vue_add_data(vue_options, { progress_title: '' }); // for progress-dialog
vue_add_global_components(components_bootstrap);
vue_add_global_components(components_utils);

/* add additional components */
  
window.vue = new Vue( vue_options );
