# Node Server
## Development
1. `npm install`
2. `npm run dev`
3. Visualization is available at [localhost:3000](localhost:3000), music player at [localhost:3000/player](localhost:3000/player)
## Deployment
1. Navigate to root directory of this repo
2. `heroku login` if not already logged in to Heroku
3. `heroku git:remote -a hack-and-roll-2021`
4. `git subtree push --prefix node-app heroku master`