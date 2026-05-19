# AFL++ Dashboard Setup

This directory contains a Docker-based monitoring stack for visualizing AFL fuzzing statistics in Grafana using StatsD and Prometheus.

StatsD listens on UDP port `8125`, so ensure this port is open on the server running the stack.
It includes:

* Grafana dashboards (`dashboards.json`)
* Prometheus configuration (`prometheus.yml`)
* StatsD → Prometheus mapping (`statsd_mapping.yml`)
* Docker Compose setup (`docker-compose.yml`)
---
## Installation

### 1. Install Docker

```bash
curl -fsSL https://get.docker.com -o get-docker.sh
sh get-docker.sh
```

### 2. Install Docker Compose
```bash
sudo apt install docker-compose
```

### 3. Start the stack
```bash
sudo docker-compose up -d
```

## Environment Variables (AFL StatsD)

Enable AFL StatsD reporting:

```bash
export AFL_STATSD=1
export AFL_STATSD_HOST=127.0.0.1
export AFL_STATSD_PORT=8125
export AFL_STATSD_TAGS_FLAVOR=dogstatsd
```

## Access Grafana

Once the stack is running, open:

```text
http://127.0.0.1:3000
```

### Default Login
* Username: `admin`
* Password: `admin`

### Add Prometheus Data Source

1. Open Grafana
2. Navigate to **Connections → Data sources → Add data source**
2. Select Prometheus
3. Set URL: http://127.0.0.1:9090
4. Click **Save & test**

### Import Dashboards
Prebuilt dashboards are included in `dashboards.json`.
1. Go to **Dashboards → Import**
2. Upload `dashboards.json`
