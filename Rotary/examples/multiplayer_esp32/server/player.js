const entity = require('./entity')

class player extends entity{

    constructor(name, socketid) {
        super()

        this.name = name
        this.socketid = socketid
        this.state = {}
    }

    static create(name, socketid) {
        const p = new player(name, socketid)
        p.spawn()
        return p
    }
    
    update_on_input(data) {
        let ro = data.encoders; 

        // for test purposes    
        // let same = JSON.stringify(this.state[1]) == JSON.stringify(ro[1]);
        // console.log("comparison " + same)

        if(!(JSON.stringify(this.state[0]) == JSON.stringify(ro[0]))) {            
        
            if (ro[0].result === 16) {
                this.position.x = this.position.x + 5
            } else if (ro[0].result === 32) {
                this.position.x = this.position.x - 5
            } else {
                this.position.x = this.position.x
            }
            this.state = ro;
        }

        if(!(JSON.stringify(this.state[1]) == JSON.stringify(ro[1]))) {            
        
            if (ro[1].result === 32) {
                this.position.y = this.position.y + 5
            } else if (ro[1].result === 16) {
                this.position.y = this.position.y - 5
            } else {
                this.position.y = this.position.y
            }
            this.state = ro;
        }        
    }

    spawn() {
        this.position = {x: 300 * Math.random(), y: 300 * Math.random()}
        this.color = this.get_random_color()
    }
    
    get_random_color() {
        var letters = '0123456789ABCDEF';
        var color = '#';
        for (var i = 0; i < 6; i++) {
          color += letters[Math.floor(Math.random() * 16)];
        }
        return color;
    }
}

module.exports = player