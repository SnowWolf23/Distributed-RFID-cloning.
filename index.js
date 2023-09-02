const express = require('express');
const pg = require('pg');

const app = express();
const port = 3000;

const config = {
  host: process.env.HOST,
  user: process.env.USER,     
  password: process.env.PASSWORD,
  database: process.env.DATABASE,
  port: 5432,
  ssl: {rejectUnauthorized: false}
};

app.get('/save_data', (req, res) => {
  const { uid, data} = req.query;

  if (!uid || !data) {
    res.send(400, 'Missing query parameters');
    return;
  }

  const client = new pg.Client(config);

  client.connect(err => {
    if (err) {
        console.error('Connection error', err.stack);
        res.send(500, 'Connection error');
    } else {
        client.query(`INSERT INTO data (uid, data) VALUES ('${uid}', '${data}')`, (err, result) => {
            if (err) {
                console.error('Error executing query', err.stack);
                res.send(500, 'Error executing query');
            } else {
                console.log('Data saved successfully');
                res.send(200, 'Data saved successfully');
            }
        });
    }
  });
});


app.get('/get_all_data', (req, res) => {
  const client = new pg.Client(config);

  client.connect(err => {
    if (err) {
        console.error('Connection error', err.stack);
        res.send(500, 'Connection error');
    } else {
        client.query(`SELECT *FROM data`, (err, result) => {
            if (err) {
                console.error('Error getting data', err.stack);
                res.send(500, 'Error getting data');
            } else {
                res.send(200, result.rows);
            }
        });
    }
  });
});

app.get('/get_serial_data', (req, res) => {
  const { serial } = req.query;

  if (!serial) {
    res.send(400, 'Missing query parameters');
    return;
  }

  const client = new pg.Client(config);

  client.connect(err => {
    if (err) {
        console.error('Connection error', err.stack);
        res.send(500, 'Connection error');
    } else {
        client.query(`SELECT *FROM data WHERE serial_no = '${serial}'`, (err, result) => {
            if (err) {
                console.error('Error getting data', err.stack);
                res.send(500, 'Error getting data');
            } else {
                res.send(200, result.rows[0]["data"]);
            }
        });
    }
  });
});

app.get('/', (req, res) => {
  res.send(`
    <h1>Available routes</h1>
    <ul>
      <li>/save_data?uid=123&data=hello</li>
      <li>/get_all_data</li>
      <li>/get_serial_data?serial=123</li>
    </ul>
   `);
});

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
});