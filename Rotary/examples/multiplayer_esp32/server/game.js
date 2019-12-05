const player = require('./player')

class game {
    constructor() {
        this.clients = new Map()
        this.players = new Map()
        this.browsers = new Map()

        this.lastUpdateTime = 0
    }

    static create() {
        const g = new game()
        g.init()
        return g
    }

    init() {
        this.lastUpdateTime = Date.now()
    }

    add_player(name, socket) {
        this.clients.set(socket.id, socket)
        this.players.set(socket.id, player.create(name, socket.id)) 
    }

    add_browser(socket) {
        this.browsers.set(socket.id, socket)
    }

    remove_player(socketid) {
        if (this.clients.has(socketid)) {
            this.clients.delete(socketid)
        }
        if (this.players.has(socketid)) {
            const p = this.players.get(socketid)
            this.players.delete(socketid)
            return p.name
        }
    }

    update_player_on_input(socketid, data) {
        const p = this.players.get(socketid)
        if (p) {
            p.update_on_input(data)
        }
    }

    send_state() {
        const players = [...this.players.values()]
        
        // console.log('server ' + JSON.stringify(players))
        this.browsers.forEach((browser, socketid) => {
            this.browsers.get(socketid).emit('state update', 
               JSON.stringify(players))
        })
    }

    send_init_state(socketid) {
        const players = [...this.players.values()]        
        
        this.browsers.get(socketid).emit('state init', 
            JSON.stringify(players))
    }

    // TODO
    // player update time procedure
}

module.exports = game