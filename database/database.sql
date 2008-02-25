--
-- PostgreSQL database dump
--

CREATE SEQUENCE game_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;

--
-- Name: game_players; Type: TABLE; Schema: public; Owner: raphael; Tablespace: 
--

CREATE TABLE game_players (
    game_id integer,
    username character varying,
    score character varying,
    role character varying
);

--
-- Name: games; Type: TABLE; Schema: public; Owner: raphael; Tablespace: 
--

CREATE TABLE games (
    game_id integer,
    category character varying,
    time_stamp integer,
    history character varying
);

--
-- Name: player_rating; Type: TABLE; Schema: public; Owner: raphael; Tablespace: 
--

CREATE TABLE player_rating (
    username character varying,
    category character varying,
    rating integer,
    volatility numeric,
    wins integer,
    draws integer,
    losses integer
);

--
-- Name: player_type; Type: TABLE; Schema: public; Owner: raphael; Tablespace: 
--

CREATE TABLE player_type (
    username character varying,
    "type" character varying
);

--
-- Name: game_players_game_id; Type: INDEX; Schema: public; Owner: raphael; Tablespace: 
--

CREATE INDEX game_players_game_id ON game_players USING btree (game_id);


--
-- Name: game_players_username; Type: INDEX; Schema: public; Owner: raphael; Tablespace: 
--

CREATE INDEX game_players_username ON game_players USING btree (username);


--
-- Name: games_game_id; Type: INDEX; Schema: public; Owner: raphael; Tablespace: 
--

CREATE UNIQUE INDEX games_game_id ON games USING btree (game_id);


--
-- Name: player_rating_key; Type: INDEX; Schema: public; Owner: raphael; Tablespace: 
--

CREATE UNIQUE INDEX player_rating_key ON player_rating USING btree (username, category);

CREATE INDEX player_rating_username ON player_rating USING btree (username);


--
-- Name: player_type_username_index; Type: INDEX; Schema: public; Owner: raphael; Tablespace: 
--

CREATE UNIQUE INDEX player_type_username_index ON player_type USING btree (username);
