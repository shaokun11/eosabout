import React, {Component} from 'react';
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs';
import Eos from 'eosjs';

ScatterJS.plugins(new ScatterEOS());


const network = {
    blockchain: 'eos',
    protocol: 'https',
    host: 'api-kylin.eosasia.one',
    port: 443,
    chainId: '5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191'
};

class App extends Component {
    state = {
        rows: [],
        scatter: null
    };

    componentDidMount() {
        setTimeout(() => {
            this.init()
        }, 1000)
    }

    takeAction(action, params) {
        console.log(action, params);
        const requiredFields = {accounts: [network]};
        this.state.scatter.getIdentity(requiredFields).then(() => {
            const account = this.state.scatter.identity.accounts.find(x => x.blockchain === 'eos');
            console.log(account)
            const eosOptions = {expireInSeconds: 60};
            const eos = this.state.scatter.eos(network, Eos, eosOptions);
            const transactionOptions = {authorization: [`${account.name}@${account.authority}`]};
            eos.contract("shaokun11111").then(ins => {
                return ins[action](account.name, ...params, transactionOptions)
            }).then(res => {
                console.log(res);
            }).catch(error => {
                console.error(error);
            });

        }).catch(error => {
            console.error("tack action", error);
        });
    }

    init() {
        ScatterJS.scatter.connect('shaokun').then(connected => {
            if (!connected) return false;
            console.log("scatter load success");
            const scatter = ScatterJS.scatter;
            this.setState({
                scatter
            });

        }).catch(err => {
            console.log(err)
        });
    }

    showtable() {
        this.state.scatter.eos(network, Eos).getTableRows({
            code: "shaokun11111",
            scope: "shaokun11111",
            table: "person",
            json: true
        }).then(res => {
                console.log(res);
                this.setState({
                    rows: res.rows
                })
            })
    }
    render() {
        return (
            <div>
                <div>
                    <button onClick={() => {
                        const num = Math.floor(Math.random() * 100000);
                        this.takeAction("upsert", ["my name is  " + num, num])
                    }}>upsert</button>
                    <button onClick={() =>   this.takeAction("erase",[])}>erase</button>
                    <button onClick={() => this.showtable()}>show table</button>
                </div>
                <div>
                    <p>below is data</p>
                    <ul>
                        {
                            this.state.rows.map((row, index) => {
                                return <li key={index}>
                                        {JSON.stringify(row)}
                                </li>
                            })
                        }
                    </ul>
                </div>
            </div>
        );
    }
}

export default App;
